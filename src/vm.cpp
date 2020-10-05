#include <chrono>
#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include <fstream>
#include <streambuf>

#include "Parser.h"
#include "common.h"
#include "debug.h"
#include "scy/pluga/pluga.h"
#include "scy/pluga/plugin_api.h"
#include "scy/pluga/sharedlibrary.h"
#include "vm.hpp"
#include "library/io.h"
#include "library/os.h"
#include "library/array.h"
#include "library/module.h"
#include "library/sfml.h"
#include "foxely.h"


VM VM::m_oInstance = VM();

using Standard = std::pair<std::string, fox::pluga::IModule>;

std::vector<std::string> standardLib = 
{
	"os",
	"io",
	"array",
	"math",
	"sfml",
};

#define TRACE2(arg1) char arg1; \
                            FOX_MODULE_CALL(arg1);
#define TRACE(arg1,...)  TRACE2(arg1)

void VM::LoadStandard(const std::string& name)
{
	NativeMethods methods;

	OSPlugin os;
	IOPlugin io;
	ArrayPlugin array;
	// SFMLPlugin sfml;

	if (name == "os")
	{
		methods = os.GetMethods();
		DefineLib(os.GetClassName(), methods);
	}
	else if (name == "io")
	{
		methods = io.GetMethods();
		DefineLib(io.GetClassName(), methods);
	}
	else if (name == "array")
	{
		methods = array.GetMethods();
		DefineNativeClass(array.GetClassName(), methods);
	}
	else if (name == "sfml")
	{
		// methods = sfml.GetMethods();
		// DefineNativeClass(sfml.GetClassName(), methods);
	}
}

VM::VM()
{
	// m_oParser.m_pVm = this;
	// openUpvalues = NULL;
	// stack[0] = Value();
	// ResetStack();
	// DefineNative("clock", clockNative);
	// initString = NULL;
	// initString = m_oParser.CopyString("init");
	// Load();
}

VM::~VM()
{
	initString = NULL;
}

VM *VM::GetInstance()
{
	return &m_oInstance;
}

void VM::ResetStack() {
	stackTop = stack;
	frameCount = 0;
}

void VM::Push(Value value) {
	*stackTop = value;
	stackTop++;
}

Value VM::Pop() {
	stackTop--;
	return *stackTop;
}

Value VM::Peek(int distance) {
	return stackTop[-1 - distance];
}

Value VM::PeekStart(int distance)
{
	if (!isInit)
		return stack[distance];
	return stack[distance + 1];
}

bool IsFalsey(Value value) {
	return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

void VM::RuntimeError(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	for (int i = frameCount - 1; i >= 0; i--) {
		CallFrame *frame = &frames[i];
		ObjectFunction *function = frame->closure->function;

		// -1 because the IP is sitting on the next instruction to be executed.
		size_t instruction = frame->ip - function->chunk.m_vCode.begin() - 1;
		fprintf(stderr, "[line %d] in ", function->chunk.m_vLines[instruction]);
		if (function->name == NULL)
			fprintf(stderr, "script\n");
		else
			fprintf(stderr, "%s()\n", function->name->string.c_str());
	}

	// ResetStack();
	result = InterpretResult::INTERPRET_RUNTIME_ERROR;
}

bool VM::Call(ObjectClosure *closure, int argCount)
{
	// Check the number of args pass to the function call
	if (argCount != closure->function->arity) {
		RuntimeError(
			"Expected %d arguments but got %d.",
			closure->function->arity,
			argCount);
		return false;
	}

	if (frameCount == FRAMES_MAX) {
		RuntimeError("Stack overflow.");
		return false;
	}

	CallFrame *frame = &frames[frameCount++];
	frame->closure = closure;
	frame->ip = closure->function->chunk.m_vCode.begin();

	frame->slots = stackTop - argCount - 1;
	return true;
}

bool VM::CallValue(Value callee, int argCount)
{
	if (IS_OBJ(callee))
	{
		switch (OBJ_TYPE(callee))
		{
		case OBJ_NATIVE:
		{
			NativeFn native = AS_NATIVE(callee);
			Value result = native(argCount, stackTop - argCount);
			stackTop -= argCount + 1;
			Push(result);
			return true;
		}

		case OBJ_BOUND_METHOD:
		{
			ObjectBoundMethod *bound = AS_BOUND_METHOD(callee);
			stackTop[-argCount - 1] = bound->receiver;
			return Call(bound->method, argCount);
		}

		case OBJ_CLASS:
		{
			ObjectClass *klass = AS_CLASS(callee);
			stackTop[-argCount - 1] = OBJ_VAL(new ObjectInstance(klass));
			Value initializer;
			if (klass->methods.Get(initString, initializer))
				return Call(AS_CLOSURE(initializer), argCount);
			else if (argCount != 0)
			{
				RuntimeError("Expected 0 arguments but got %d.", argCount);
				return false;
			}
			return true;
		}

		case OBJ_NATIVE_CLASS:
		{
			ObjectNativeClass *klass = AS_NATIVE_CLASS(callee);
			stackTop[-argCount - 1] = OBJ_VAL(new ObjectNativeInstance(klass));
			
			Value initializer;

			if (klass->methods.Get(initString, initializer))
			{
				NativeFn native = AS_NATIVE(initializer);
				native(argCount, stackTop - argCount);
				stackTop -= argCount;
			}
			return true;
		}

		case OBJ_CLOSURE:
			return Call(AS_CLOSURE(callee), argCount);

		default:
			// Non-callable object type.
			break;
		}
	}

	RuntimeError("Can only call functions and classes.");
	return false;
}

ObjectUpvalue *VM::CaptureUpvalue(Value *local)
{
	ObjectUpvalue *prevUpvalue = NULL;
	ObjectUpvalue *upvalue = openUpvalues;

	while (upvalue != NULL && upvalue->location > local) {
		prevUpvalue = upvalue;
		upvalue = upvalue->next;
	}

	if (upvalue != NULL && upvalue->location == local)
		return upvalue;
	ObjectUpvalue *createdUpvalue = new ObjectUpvalue(local);
	if (prevUpvalue == NULL) {
		openUpvalues = createdUpvalue;
	} else {
		prevUpvalue->next = createdUpvalue;
	}
	return createdUpvalue;
}

void VM::CloseUpvalues(Value *last)
{
	while (openUpvalues != NULL && openUpvalues->location >= last) {
		ObjectUpvalue *upvalue = openUpvalues;
		upvalue->closed = *upvalue->location;
		upvalue->location = &upvalue->closed;
		openUpvalues = upvalue->next;
	}
}

void VM::DefineNative(const std::string &name, NativeFn function)
{
	Push(OBJ_VAL(m_oParser.CopyString(name)));
	Push(OBJ_VAL(new ObjectNative(function)));
	globals.Set(AS_STRING(PeekStart(0)), PeekStart(1));
	Pop();
	Pop();
}

void VM::DefineLib(const std::string &name, NativeMethods &functions)
{
	Push(OBJ_VAL(m_oParser.CopyString(name)));
	Push(OBJ_VAL(new ObjectLib(AS_STRING(PeekStart(0)))));
	globals.Set(AS_STRING(PeekStart(0)), PeekStart(1));
	ObjectLib *klass = AS_LIB(Pop());
	Pop();
	Push(OBJ_VAL(klass));
	for (auto &it : functions)
	{
		Push(OBJ_VAL(m_oParser.CopyString(it.first)));
		Push(OBJ_VAL(new ObjectNative(it.second)));

		klass->methods.Set(AS_STRING(PeekStart(1)), PeekStart(2));

		Pop();
		Pop();
	}
	Pop();
}

void VM::DefineNativeClass(const std::string &name, NativeMethods &functions)
{
	Push(OBJ_VAL(m_oParser.CopyString(name)));
	Push(OBJ_VAL(new ObjectNativeClass(AS_STRING(PeekStart(0)))));
	globals.Set(AS_STRING(PeekStart(0)), PeekStart(1));
	ObjectNativeClass *klass = AS_NATIVE_CLASS(Pop());
	Pop();
	Push(OBJ_VAL(klass));
	for (auto &it : functions) {
		NativeFn func = it.second;

		Push(OBJ_VAL(m_oParser.CopyString(it.first)));
		Push(OBJ_VAL(new ObjectNative(func)));

		klass->methods.Set(AS_STRING(PeekStart(1)), PeekStart(2));

		Pop();
		Pop();
	}
	Pop();
}

bool VM::InvokeFromClass(ObjectClass *klass, ObjectString *name, int argCount)
{
	Value method;
	if (!klass->methods.Get(name, method))
	{
		RuntimeError("Undefined property '%s'.", name->string.c_str());
		return false;
	}

	return Call(AS_CLOSURE(method), argCount);
}

bool VM::InvokeFromNativeClass(ObjectNativeClass *klass, ObjectString *name, int argCount)
{
	Value method;
	if (!klass->methods.Get(name, method))
	{
		RuntimeError("Undefined property '%s'.", name->string.c_str());
		return false;
	}
	return CallValue(method, argCount);
}

bool VM::Invoke(ObjectString *name, int argCount)
{
	Value receiver = Peek(argCount);

	if (!IS_INSTANCE(receiver) && !IS_NATIVE_INSTANCE(receiver) && !IS_LIB(receiver))
	{
		RuntimeError("Only instances && module have methods.");
		return false;
	}

	Value value;
	if (IS_INSTANCE(receiver)) {
		ObjectInstance *instance = AS_INSTANCE(receiver);

		if (instance->fields.Get(name, value)) {
			stackTop[-argCount - 1] = value;
			return CallValue(value, argCount);
		}

		return InvokeFromClass(instance->klass, name, argCount);
	}
	else if (IS_NATIVE_INSTANCE(receiver))
	{
		ObjectNativeInstance *instance = AS_NATIVE_INSTANCE(receiver);

		if (instance->fields.Get(name, value))
		{
			stackTop[-argCount - 1] = value;
			return CallValue(value, argCount);
		}
		return InvokeFromNativeClass(instance->klass, name, argCount);
	}
	else if (IS_LIB(receiver))
	{
		ObjectLib *instance = AS_LIB(receiver);
		Value method;
		if (!instance->methods.Get(name, method)) {
			RuntimeError("Undefined property '%s'.", name->string.c_str());
			return false;
		}
		return CallValue(method, argCount);
	}
}

InterpretResult VM::interpret(const char *source)
{
	if (!isInit)
	{
		m_oParser.m_pVm = this;
		openUpvalues = NULL;
		stack[0] = Value();
		ResetStack();
		DefineNative("clock", clockNative);
		initString = NULL;
		initString = m_oParser.CopyString("init");
		ModulePlugin module;
		NativeMethods methods = module.GetMethods();
		DefineLib(module.GetClassName(), methods);
		manager.GetAllLibrary();
		isInit = true;
	}

	Chunk oChunk;
	ObjectFunction *function = Compile(m_oParser, source, &oChunk);

	if (function == NULL)
		return (INTERPRET_COMPILE_ERROR);

	Push(OBJ_VAL(function));
	ObjectClosure *closure = new ObjectClosure(function);
	Pop();
	Push(OBJ_VAL(closure));
	CallValue(OBJ_VAL(closure), 0);
	InterpretResult result = run();

	return result;
}

InterpretResult VM::run() {
	CallFrame *frame = &frames[frameCount - 1];

#define READ_BYTE() (*frame->ip++)
#define READ_CONSTANT()                                                        \
	(frame->closure->function->chunk.m_oConstants.m_vValues[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define READ_SHORT()                                                           \
	(frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))

#define BINARY_OP(valueType, op)                                               \
	do {                                                                       \
		if (!IS_NUMBER(Peek(0)) || !IS_NUMBER(Peek(1))) {                      \
			RuntimeError("Operands must be numbers.");                         \
			return INTERPRET_RUNTIME_ERROR;                                    \
		}                                                                      \
		double b = AS_NUMBER(Pop());                                           \
		double a = AS_NUMBER(Pop());                                           \
		Push(valueType(a op b));                                               \
	} while (false)

	for (;;) {
		if (result == INTERPRET_RUNTIME_ERROR)
			return result;
#ifdef DEBUG_TRACE_EXECUTION
		printf("          ");
		for (Value *slot = stack; slot < stackTop; slot++) {
			printf("[ ");
			PrintValue(*slot);
			printf(" ]");
		}
		printf("\n");
		disassembleInstruction(
			frame->closure->function->chunk,
			(int)(frame->ip - frame->closure->function->chunk.m_vCode.begin()));
#endif
		uint8_t instruction;
		switch (instruction = READ_BYTE())
		{
		case OP_NIL:
			Push(NIL_VAL);
			break;
		case OP_TRUE:
			Push(BOOL_VAL(true));
			break;
		case OP_FALSE:
			Push(BOOL_VAL(false));
			break;
		case OP_POP:
			Pop();
			break;

		case OP_GET_UPVALUE: {
			uint8_t slot = READ_BYTE();
			Push(*frame->closure->upValues[slot]->location);
			break;
		}

		case OP_SET_UPVALUE: {
			uint8_t slot = READ_BYTE();
			*frame->closure->upValues[slot]->location = Peek(0);
			break;
		}

		case OP_GET_LOCAL: {
			uint8_t slot = READ_BYTE();
			Push(frame->slots[slot]);
			break;
		}

		case OP_SET_LOCAL:
		{
			uint8_t slot = READ_BYTE();
			frame->slots[slot] = Peek(0);
			break;
		}

		case OP_GET_GLOBAL:
		{
			ObjectString *name = READ_STRING();
			Value value;
			if (!globals.Get(name, value)) {
				RuntimeError("Undefined variable '%s'.", name->string.c_str());
				return INTERPRET_RUNTIME_ERROR;
			}
			Push(value);
			break;
		}

		case OP_DEFINE_GLOBAL: {
			ObjectString *name = READ_STRING();
			globals.Set(name, Peek(0));
			Pop();
			break;
		}

		case OP_SET_GLOBAL: {
			ObjectString *name = READ_STRING();
			if (globals.Set(name, Peek(0))) {
				globals.Delete(name);
				RuntimeError("Undefined variable '%s'.", name->string.c_str());
				return INTERPRET_RUNTIME_ERROR;
			}
			break;
		}

		case OP_GET_PROPERTY: {
			if (!IS_INSTANCE(Peek(0))) {
				RuntimeError("Only instances have properties.");
				return INTERPRET_RUNTIME_ERROR;
			}

			ObjectInstance *instance = AS_INSTANCE(Peek(0));
			ObjectString *name = READ_STRING();

			Value value;
			if (instance->fields.Get(name, value)) {
				Pop(); // Instance.
				Push(value);
				break;
			}

			if (!BindMethod(instance->klass, name)) {
				return INTERPRET_RUNTIME_ERROR;
			}
			break;
		}

		case OP_SET_PROPERTY: {
			if (!IS_INSTANCE(Peek(1)))
			{
				RuntimeError("Only instances have fields.");
				return INTERPRET_RUNTIME_ERROR;
			}

			ObjectInstance *instance = AS_INSTANCE(Peek(1));
			instance->fields.Set(READ_STRING(), Peek(0));

			Value value = Pop();
			Pop();
			Push(value);
			break;
		}

		case OP_EQUAL: {
			Value b = Pop();
			Value a = Pop();
			Push(BOOL_VAL(ValuesEqual(a, b)));
			break;
		}

		case OP_GREATER:
			BINARY_OP(BOOL_VAL, >);
			break;
		case OP_LESS:
			BINARY_OP(BOOL_VAL, <);
			break;
		case OP_ADD: {
			if (IS_STRING(Peek(0)) && IS_STRING(Peek(1))) {
				Concatenate();
			} else if (IS_NUMBER(Peek(0)) && IS_NUMBER(Peek(1))) {
				double b = AS_NUMBER(Pop());
				double a = AS_NUMBER(Pop());
				Push(NUMBER_VAL(a + b));
			} else {
				RuntimeError("Operands must be two numbers or two strings.");
				return INTERPRET_RUNTIME_ERROR;
			}
			break;
		}
		case OP_SUB:
			BINARY_OP(NUMBER_VAL, -);
			break;
		case OP_MUL:
			BINARY_OP(NUMBER_VAL, *);
			break;
		case OP_DIV:
			BINARY_OP(NUMBER_VAL, /);
			break;
		case OP_NOT:
			Push(BOOL_VAL(IsFalsey(Pop())));
			break;
		case OP_NEGATE: {
			if (!IS_NUMBER(Peek(0))) {
				RuntimeError("Operand must be a number.");
				return INTERPRET_RUNTIME_ERROR;
			}

			Push(NUMBER_VAL(-AS_NUMBER(Pop())));
			break;
		}
		case OP_PRINT: {
			PrintValue(Pop());
			std::cout << std::endl;
			break;
		}

		case OP_JUMP: {
			uint16_t offset = READ_SHORT();
			frame->ip += offset;
			break;
		}

		case OP_JUMP_IF_FALSE: {
			uint16_t offset = READ_SHORT();
			if (IsFalsey(Peek(0)))
				frame->ip += offset;
			break;
		}

		case OP_LOOP: {
			uint16_t offset = READ_SHORT();
			frame->ip -= offset;
			break;
		}

		case OP_CALL: {
			int argCount = READ_BYTE();
			if (!CallValue(Peek(argCount), argCount))
				return INTERPRET_RUNTIME_ERROR;
			frame = &frames[frameCount - 1];
			break;
		}

		case OP_CLASS:
			Push(OBJ_VAL(new ObjectClass(READ_STRING())));
			break;

		case OP_INHERIT: {
			Value superclass = Peek(1);

			if (!IS_CLASS(superclass)) {
				RuntimeError("Superclass must be a class.");
				return INTERPRET_RUNTIME_ERROR;
			}
			ObjectClass *subclass = AS_CLASS(Peek(0));
			subclass->methods.AddAll(AS_CLASS(superclass)->methods);
			subclass->superClass = AS_CLASS(superclass);
			subclass->derivedCount = AS_CLASS(superclass)->derivedCount + 1;
			Pop(); // Subclass.
			break;
		}

		case OP_METHOD:
			DefineMethod(READ_STRING());
			break;

		case OP_INVOKE: {
			ObjectString *method = READ_STRING();
			int argCount = READ_BYTE();
			if (!Invoke(method, argCount)) {
				return INTERPRET_RUNTIME_ERROR;
			}
			frame = &frames[frameCount - 1];
			break;
		}

		case OP_SUPER_INVOKE: {
			ObjectString *method = READ_STRING();
			int argCount = READ_BYTE();
			ObjectClass *superclass = AS_CLASS(Pop());
			if (!InvokeFromClass(superclass, method, argCount)) {
				return INTERPRET_RUNTIME_ERROR;
			}
			frame = &frames[frameCount - 1];
			break;
		}

		case OP_CLOSURE: {
			ObjectFunction *function = AS_FUNCTION(READ_CONSTANT());
			ObjectClosure *closure = new ObjectClosure(function);
			Push(OBJ_VAL(closure));

			for (int i = 0; i < closure->upvalueCount; i++) {
				uint8_t isLocal = READ_BYTE();
				uint8_t index = READ_BYTE();
				if (isLocal) {
					closure->upValues[i] = CaptureUpvalue(frame->slots + index);
				} else {
					closure->upValues[i] = frame->closure->upValues[index];
				}
			}
			break;
		}

		case OP_CLOSE_UPVALUE:
			CloseUpvalues(stackTop - 1);
			Pop();
			break;

		case OP_GET_SUPER: {
			ObjectString *name = READ_STRING();
			ObjectClass *superclass = AS_CLASS(Pop());
			if (!BindMethod(superclass, name)) {
				return INTERPRET_RUNTIME_ERROR;
			}
			break;
		}

		case OP_IMPORT:
		{
			ObjectString *path = READ_STRING();
			std::string file = path->string + ".fox";
			if (!imports.Set(path, NIL_VAL) || !imports.Set(m_oParser.TakeString(file.c_str()), NIL_VAL))
        		break;
			std::vector<std::string>::iterator it = std::find(standardLib.begin(), standardLib.end(), path->string);
			if (it != standardLib.end())
			{
				LoadStandard(*it);
				break;
			}

			size_t lastindex = path->string.find_last_of("."); 
			std::string rawname = path->string.substr(0, lastindex); 

			if (manager.LoadLibrary(rawname))
				break;

			std::ifstream t(file);
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

			Chunk oChunk;
			ObjectFunction *function = Compile(m_oParser, str.c_str(), &oChunk);
			if (function == NULL) {
				RuntimeError("Can't find '%s'.", file.c_str());
				return INTERPRET_RUNTIME_ERROR;
			}
			Push(OBJ_VAL(function));
			ObjectClosure *closure = new ObjectClosure(function);
			Pop();
			Call(closure, 0);
			frame = &frames[frameCount - 1];
			break;
		}

		case OP_RETURN: {
			Value result = Pop();
			CloseUpvalues(frame->slots);
			frameCount--;
			if (frameCount == 0) {
				Pop();
				return INTERPRET_OK;
			}

			stackTop = frame->slots;
			Push(result);

			frame = &frames[frameCount - 1];
			break;
		}

		case OP_CONST: {
			Value constant = READ_CONSTANT();
			Push(constant);
			break;
		}
		}
		// auto endExec = std::chrono::steady_clock::now();
		// disassembleInstruction(frame->function->chunk, (int)(frame->ip -
		// frame->function->chunk.m_vCode.begin())); std::cout << "Instruction
		// Time: " << std::chrono::duration <double> (endExec -
		// startExec).count() << " s" << std::endl;
	}

#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP
}

void VM::Concatenate() {
	ObjectString *b = AS_STRING(Peek(0));
	ObjectString *a = AS_STRING(Peek(1));
	Pop();
	Pop();

	ObjectString *result = m_oParser.TakeString(a->string + b->string);
	Push(OBJ_VAL(result));
}

void VM::AddToRoots() {
	for (Value *slot = stack; slot < stackTop; slot++) {
		AddValueToRoot(*slot);
	}

	for (int i = 0; i < frameCount; i++) {
		AddObjectToRoot((Object *)frames[i].closure);
	}

	for (ObjectUpvalue *upvalue = openUpvalues; upvalue != NULL;
		 upvalue = upvalue->next) {
		AddObjectToRoot((Object *)upvalue);
	}

	AddTableToRoot(globals);
	AddCompilerToRoots();
	AddObjectToRoot((Object *)initString);
}

void VM::AddTableToRoot(Table &table) {
	for (int i = 0; i < table.m_iCapacity; i++) {
		Entry &entry = table.m_vEntries[i];
		AddObjectToRoot((Object *)entry.m_pKey);
		AddValueToRoot(entry.m_oValue);
	}
}

void VM::AddValueToRoot(Value value) {
	if (!IS_OBJ(value))
		return;
	AddObjectToRoot(AS_OBJ(value));
}

void VM::AddObjectToRoot(Object *object) {
	if (object == NULL)
		return;
#ifdef DEBUG_LOG_GC
	printf("%p added to root ", (void *)object);
	PrintValue(OBJ_VAL(object));
	printf("\n");
#endif
	GC::Gc.AddRoot(object);

	BlackenObject(object);
	strings.RemoveWhite();
}

void VM::AddCompilerToRoots() {
	Compiler *compiler = m_oParser.currentCompiler;
	while (compiler != NULL) {
		AddObjectToRoot((Object *)compiler->function);
		compiler = compiler->enclosing;
	}
}

void VM::AddArrayToRoot(ValueArray *array) {
	for (int i = 0; i < array->m_vValues.size(); i++) {
		AddValueToRoot(array->m_vValues[i]);
	}
}

void VM::BlackenObject(Object *object) {
#ifdef DEBUG_LOG_GC
	printf("%p blacken ", (void *)object);
	PrintValue(OBJ_VAL(object));
	printf("\n");
#endif
	switch (object->type) {
	case OBJ_INSTANCE: {
		ObjectInstance *instance = (ObjectInstance *)object;
		AddObjectToRoot((Object *)instance->klass);
		AddTableToRoot(instance->fields);
		break;
	}
	case OBJ_BOUND_METHOD: {
		ObjectBoundMethod *bound = (ObjectBoundMethod *)object;
		AddValueToRoot(bound->receiver);
		AddObjectToRoot((Object *)bound->method);
		break;
	}
	case OBJ_CLASS: {
		ObjectClass *klass = (ObjectClass *)object;
		AddObjectToRoot((Object *)klass->name);
		AddTableToRoot(klass->methods);
		break;
	}
	case OBJ_CLOSURE: {
		ObjectClosure *closure = (ObjectClosure *)object;
		AddObjectToRoot((Object *)closure->function);
		for (int i = 0; i < closure->upvalueCount; i++) {
			AddObjectToRoot((Object *)closure->upValues[i]);
		}
		break;
	}
	case OBJ_FUNCTION: {
		ObjectFunction *function = (ObjectFunction *)object;
		AddObjectToRoot((Object *)function->name);
		AddArrayToRoot(&function->chunk.m_oConstants);
		break;
	}
	case OBJ_UPVALUE:
		AddValueToRoot(((ObjectUpvalue *)object)->closed);
		break;
	case OBJ_NATIVE:
	case OBJ_STRING:
		break;
	}
}

void VM::DefineMethod(ObjectString *name) {
	Value method = Peek(0);
	ObjectClass *klass = AS_CLASS(Peek(1));
	klass->methods.Set(name, method);
	Pop();
}

bool VM::BindMethod(ObjectClass *klass, ObjectString *name) {
	Value method;
	if (!klass->methods.Get(name, method)) {
		RuntimeError("Undefined property '%s'.", name->string.c_str());
		return false;
	}

	ObjectBoundMethod *bound = new ObjectBoundMethod(Peek(0), AS_CLOSURE(method));
	Pop();
	Push(OBJ_VAL(bound));
	return true;
}