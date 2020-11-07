#include <chrono>
#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include <fstream>
#include <streambuf>

#include "common.h"
#include "debug.h"
#include "scy/pluga/pluga.h"
#include "scy/pluga/plugin_api.h"
#include "scy/pluga/sharedlibrary.h"
#include "library/library.h"
#include "library/sfml.h"
#include "foxely.h"
#include "Parser.h"
#include "vm.hpp"
#include "object.hpp"

VM::VM() : gc(this), m_oParser(this), modules()
{
    ResetStack();
    isInit = false;
    currentModule = NULL;
    m_pApiStack = NULL;
    openUpvalues = NULL;
    DefineModule("core");
    initString = AS_STRING(NewString("init"));
    DefineCoreArray(this);
    // ResetStack();
    // ModulePlugin module(this);
    // DefineLib(module.GetClassName(), module.m_oMethods);
    // DefineNative("clock", clockNative);
}

VM::~VM()
{
    initString = NULL;
}

void VM::ResetStack()
{
    stackTop = stack;
    frameCount = 0;
}

void VM::Push(Value value)
{
    *stackTop = value;
    stackTop++;
}

Value VM::Pop()
{
    FOX_ASSERT(stackTop != NULL, "No temporary roots to release.");
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

    for (int i = frameCount - 1; i >= 0; i--)
    {
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

bool VM::CallFunction(ObjectClosure *closure, int argCount)
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
            Value result = native(this, argCount, stackTop - argCount);
            stackTop -= argCount + 1;
            Push(result);
            return true;
        }

        case OBJ_BOUND_METHOD:
        {
            ObjectBoundMethod *bound = AS_BOUND_METHOD(callee);
            stackTop[-argCount - 1] = bound->receiver;
            return CallFunction(bound->method, argCount);
        }

        case OBJ_CLASS:
        {
            ObjectClass *klass = AS_CLASS(callee);
            stackTop[-argCount - 1] = OBJ_VAL(gc.New<ObjectInstance>(klass));
            Value initializer;
            if (klass->methods.Get(initString, initializer))
                return CallFunction(AS_CLOSURE(initializer), argCount);
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
            stackTop[-argCount - 1] = OBJ_VAL(gc.New<ObjectNativeInstance>(klass));
            
            Value initializer;

            if (klass->methods.Get(initString, initializer))
            {
                NativeFn native = AS_NATIVE(initializer);
                native(this, argCount, stackTop - argCount);
                stackTop -= argCount;
            }
            return true;
        }

        case OBJ_CLOSURE:
            return CallFunction(AS_CLOSURE(callee), argCount);

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
    ObjectUpvalue *createdUpvalue = gc.New<ObjectUpvalue>(local);
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

void VM::DefineFunction(const std::string &strModule, const std::string &name, NativeFn function)
{
    Value oStrModuleName = NewString(strModule.c_str());

    // See if the module has already been loaded.
    ObjectModule* module = GetModule(oStrModuleName);
    if (module != NULL)
    {
        Push(NewString(name.c_str()));
        Push(OBJ_VAL(gc.New<ObjectNative>(function)));
        module->m_vVariables.Set(AS_STRING(PeekStart(0)), PeekStart(1));
        Pop();
        Pop();
    }
    else
        std::cerr << "'" << strModule << "': Could not find the module." << std::endl;
}

void VM::DefineLib(const std::string &strModule, const std::string &name, NativeMethods &functions)
{
    Value oStrModuleName = NewString(strModule.c_str());

    // See if the module has already been loaded.
    ObjectModule* module = GetModule(oStrModuleName);
    if (module != NULL)
    {
        Push(OBJ_VAL(m_oParser.CopyString(name)));
        Push(OBJ_VAL(gc.New<ObjectLib>(AS_STRING(PeekStart(0)))));
        module->m_vVariables.Set(AS_STRING(PeekStart(0)), PeekStart(1));
        ObjectLib *klass = AS_LIB(Pop());
        Pop();
        Push(OBJ_VAL(klass));
        for (auto &it : functions)
        {
            Push(OBJ_VAL(m_oParser.CopyString(it.first)));
            Push(OBJ_VAL(gc.New<ObjectNative>(it.second)));

            klass->methods.Set(AS_STRING(PeekStart(1)), PeekStart(2));

            Pop();
            Pop();
        }
        Pop();
    }
}

void VM::DefineModule(const std::string& strName)
{
    Value oStrName = NewString(strName.c_str());

    // See if the module has already been loaded.
    ObjectModule* module = GetModule(oStrName);
    if (module == NULL)
    {
        module = gc.New<ObjectModule>(AS_STRING(oStrName));

        // It's possible for the wrenMapSet below to resize the modules map,
        // and trigger a GC while doing so. When this happens it will collect
        // the module we've just created. Once in the map it is safe.
        Push(OBJ_VAL(module));

        // Store it in the VM's module registry so we don't load the same module
        // multiple times.
        modules.Set(AS_STRING(oStrName), OBJ_VAL(module));

        Pop();

        // Implicitly import the core module.
    //     ObjectModule* coreModule = GetModule(NewString("core"));
    //     for (int i = 0; i < coreModule->m_vVariables.m_iCount; i++)
    //         module->m_vVariables.AddAll(coreModule->m_vVariables);
    }
    else
        std::cerr << "'" << strName << "': This module already exist !!" << std::endl;
}

void VM::DefineClass(const std::string &strModule, const std::string &name, NativeMethods &functions)
{
    Value oStrModuleName = NewString(strModule.c_str());

    // See if the module has already been loaded.
    ObjectModule* module = GetModule(oStrModuleName);
    if (module != NULL)
    {
        Push(OBJ_VAL(m_oParser.CopyString(name)));
        Push(OBJ_VAL(gc.New<ObjectNativeClass>(AS_STRING(PeekStart(0)))));
        module->m_vVariables.Set(AS_STRING(PeekStart(0)), PeekStart(1));
        ObjectNativeClass *klass = AS_NATIVE_CLASS(Pop());
        Pop();
        Push(OBJ_VAL(klass));
        for (auto &it : functions) {
            NativeFn func = it.second;

            Push(OBJ_VAL(m_oParser.CopyString(it.first)));
            Push(OBJ_VAL(gc.New<ObjectNative>(func)));

            klass->methods.Set(AS_STRING(PeekStart(1)), PeekStart(2));

            Pop();
            Pop();
        }
        Pop();
    }
}

void VM::DefineBuiltIn(Table& methods, NativeMethods &functions)
{
    for (auto &it : functions)
    {
        NativeFn func = it.second;

        Push(OBJ_VAL(m_oParser.CopyString(it.first)));
        Push(OBJ_VAL(gc.New<ObjectNative>(func)));

        methods.Set(AS_STRING(PeekStart(0)), PeekStart(1));

        Pop();
        Pop();
    }
}

bool VM::InvokeFromClass(ObjectClass *klass, ObjectString *name, int argCount)
{
    Value method;
    if (!klass->methods.Get(name, method))
    {
        RuntimeError("Undefined property '%s'.", name->string.c_str());
        return false;
    }

    if (method == NIL_VAL)
    {
        RuntimeError("The class '%s' doesn't implement interface members '%s'.", klass->name->string.c_str(), name->string.c_str());
        return false;
    }

    return CallFunction(AS_CLOSURE(method), argCount);
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
    Value value;

    switch (AS_OBJ(receiver)->type)
    {
        case OBJ_INSTANCE:
        {
            ObjectInstance *instance = AS_INSTANCE(receiver);
            if (instance->fields.Get(name, value)) {
                stackTop[-argCount - 1] = value;
                return CallValue(value, argCount);
            }
            return InvokeFromClass(instance->klass, name, argCount);
        }

        case OBJ_NATIVE_INSTANCE:
        {
            ObjectNativeInstance *instance = AS_NATIVE_INSTANCE(receiver);
            if (instance->fields.Get(name, value))
            {
                stackTop[-argCount - 1] = value;
                return CallValue(value, argCount);
            }
            return InvokeFromNativeClass(instance->klass, name, argCount);
        }

        case OBJ_LIB:
        {
            ObjectLib *instance = AS_LIB(receiver);
            Value method;
            if (!instance->methods.Get(name, method))
            {
                RuntimeError("Undefined property '%s'.", name->string.c_str());
                return false;
            }
            return CallValue(method, argCount);
        }

        case OBJ_ARRAY:
        {
            Value method;
            if (!arrayMethods.Get(name, method))
            {
                RuntimeError("Undefined methods '%s'.", name->string.c_str());
                return false;
            }
            return CallValue(method, argCount);
        }
        default:
            RuntimeError("Only instances && module have methods.");
            return false;
    }
}

InterpretResult VM::Interpret(const char* module, const char* source)
{
    ResetStack();

    // if (!isInit)
    // {
    //     initString = AS_STRING(NewString("init"));
    //     DefineCoreArray(this);
    //     isInit = true;
    // }
    
    result = INTERPRET_OK;

    ObjectClosure* closure = CompileSource(module, source, false, true);
    if (closure == NULL)
        return INTERPRET_COMPILE_ERROR;
    
    Push(OBJ_VAL(closure));
    // ObjFiber* fiber = wrenNewFiber(vm, closure);
    // wrenPopRoot(vm); // closure.
    // m_pApiStack = NULL;
    CallValue(OBJ_VAL(closure), 0);

    return run();
}

ObjectClosure* VM::CompileSource(const char* module, const char* source, bool isExpression, bool printErrors)
{
    Value nameValue = NewString("core");
    if (module != NULL)
    {
        nameValue = NewString(module);
        Push(nameValue);
    }
    
    ObjectClosure* closure = CompileInModule(nameValue, source, isExpression, printErrors);

    if (module != NULL) Pop(); // nameValue.
    return closure;
}


Value VM::NewString(const char* string)
{
    return OBJ_VAL(m_oParser.TakeString(string));
}

static bool ValueIsNumber(Value oNumber)
{
    if (IS_NUMBER(oNumber))
        return true;
    if (IS_INT(oNumber))
        return true;
    return false;
}

InterpretResult VM::run()
{
    CallFrame *frame = &frames[frameCount - 1];

#define READ_BYTE() (*frame->ip++)
#define READ_CONSTANT()                                                        \
    (frame->closure->function->chunk.m_oConstants.m_vValues[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define READ_SHORT()                                                           \
    (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))

#define BINARY_OP(ValueType, type, op)                                         \
    do {                                                                       \
        if (!ValueIsNumber(Peek(0)) || !ValueIsNumber(Peek(1))) {              \
            RuntimeError("Operands must be numbers.");                         \
            return INTERPRET_RUNTIME_ERROR;                                    \
        }                                                                      \
        type b = AS_##ValueType(Pop());                                           \
        type a = AS_##ValueType(Pop());                                           \
        Push(ValueType##_VAL(a op b));                                       \
    } while (false)

    while (true) {
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
        uint8_t instruction = READ_BYTE();
        switch (instruction)
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
            if (!currentModule->m_vVariables.Get(name, value)) {
                RuntimeError("Undefined variable '%s'.", name->string.c_str());
                return INTERPRET_RUNTIME_ERROR;
            }
            Push(value);
            break;
        }

        case OP_DEFINE_GLOBAL: {
            ObjectString *name = READ_STRING();
            currentModule->m_vVariables.Set(name, Peek(0));
            Pop();
            break;
        }

        case OP_SET_GLOBAL: {
            ObjectString *name = READ_STRING();
            if (currentModule->m_vVariables.Set(name, Peek(0))) {
                currentModule->m_vVariables.Delete(name);
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
            BINARY_OP(BOOL, double, >);
            break;
        case OP_LESS:
            BINARY_OP(BOOL, double, <);
            break;
        case OP_ADD: {
            if (IS_STRING(Peek(0)) && IS_STRING(Peek(1))) {
                Concatenate();
            } else if (IS_NUMBER(Peek(0)) && IS_NUMBER(Peek(1))) {
                double b = AS_NUMBER(Pop());
                double a = AS_NUMBER(Pop());
                Push(NUMBER_VAL(a + b));
            } else if (IS_INT(Peek(0)) && IS_INT(Peek(1))) {
                int b = AS_INT(Pop());
                int a = AS_INT(Pop());
                Push(INT_VAL(a + b));
            } else {
                RuntimeError("Operands must be two numbers or two strings.");
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_SUB:
            if (IS_NUMBER(Peek(0)))
                BINARY_OP(NUMBER, double, -);
            else if (IS_INT(Peek(0)))
                BINARY_OP(INT, int, -);
            break;
        case OP_MUL:
            if (IS_NUMBER(Peek(0)))
                BINARY_OP(NUMBER, double, *);
            else if (IS_INT(Peek(0)))
                BINARY_OP(INT, int, *);
            break;
        case OP_DIV:
            if (IS_NUMBER(Peek(0)))
                BINARY_OP(NUMBER, double,  /);
            else if (IS_INT(Peek(0)))
                BINARY_OP(INT, int, /);
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
        case OP_PRINT:
        {
            int argCount = READ_BYTE();
            int tempArgCount = argCount;
            int percentCount = 0;
            Value string = Peek(--tempArgCount);
            
            for (int i = 0; AS_STRING(string)->string[i]; i++)
            {
                if (AS_STRING(string)->string[i] == '%' && AS_STRING(string)->string[i + 1] == '%')
                    i++;
                else if (AS_STRING(string)->string[i] == '%')
                    percentCount++;
            }
            
            if (tempArgCount != percentCount)
            {
                RuntimeError("Expected %d arguments but got %d in print call.", percentCount, tempArgCount);
                break;
            }
            
            for (int i = 0; AS_STRING(string)->string[i]; i++)
            {
                if (AS_STRING(string)->string[i] != '%') {
                    printf("%c", AS_STRING(string)->string[i]);
                } else if (AS_STRING(string)->string[i] == '%' && AS_STRING(string)->string[i + 1] == '%') {
                    printf("%%");
                    i++;
                } else
                    PrintValue(Peek(--tempArgCount));
            }
            stackTop -= argCount;
            break;
        }

        case OP_PRINT_REPL:
        {
            PrintValue(Pop());
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
            Push(OBJ_VAL(gc.New<ObjectClass>(READ_STRING())));
            break;
        
        case OP_INTERFACE:
            Push(OBJ_VAL(gc.New<ObjectInterface>(READ_STRING())));
            break;

        case OP_INHERIT: {
            Value superclass = Peek(1);

            if (!IS_CLASS(superclass) && !IS_INTERFACE(superclass)) {
                RuntimeError("Superclass must be a class or an interface");
                return INTERPRET_RUNTIME_ERROR;
            }

            ObjectClass *subclass = AS_CLASS(Peek(0));

            if (IS_CLASS(superclass))
            {
                subclass->methods.AddAll(AS_CLASS(superclass)->methods);
                subclass->superClass = AS_CLASS(superclass);
                subclass->derivedCount = AS_CLASS(superclass)->derivedCount + 1;
            }
            else
            {
                subclass->methods.AddAll(AS_INTERFACE(superclass)->methods);
            }
            Pop(); // Subclass.
            break;
        }

        case OP_METHOD:
            DefineMethod(READ_STRING());
            break;
        
        case OP_INTERFACE_PROCEDURE:
        {
            ObjectInterface *interface = AS_INTERFACE(Peek(0));
            interface->methods.Set(READ_STRING(), NIL_VAL);
            // Pop();
            break;
        }

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
            ObjectClosure *closure = gc.New<ObjectClosure>(this, function);
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
            // ObjectString *path = READ_STRING();
            // std::string file = path->string + ".fox";
            Push(ImportModule(OBJ_VAL(READ_STRING())));
            // if (!imports.Set(path, NIL_VAL) || !imports.Set(m_oParser.TakeString(file.c_str()), NIL_VAL))
            //     break;
            // std::vector<std::string>::iterator it = std::find(standardLib.begin(), standardLib.end(), path->string);
            // if (it != standardLib.end())
            // {
            //     LoadStandard(*it);
            //     break;
            // }

            // size_t lastindex = path->string.find_last_of("."); 
            // std::string rawname = path->string.substr(0, lastindex); 

            // if (manager.LoadLibrary(rawname))
            // 	break;

            // std::ifstream t(file);
            // std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

            // if (result == INTERPRET_RUNTIME_ERROR)
            //     break;
            if (IS_CLOSURE(Peek(0)))
            {
                CallFunction(AS_CLOSURE(Peek(0)), 0);
                frame = &frames[frameCount - 1];
            }
            else
            {
                // The module has already been loaded. Remember it so we can import
                // variables from it if needed.
                currentModule->m_vVariables.AddAll(AS_MODULE(Pop())->m_vVariables);
                // currentModule = AS_MODULE(Pop());
                // std::cerr << frameCount << " Frames::::" << std::endl;
            }
            break;
        }

        case OP_RETURN: {
            Value result = Pop();
            CloseUpvalues(frame->slots);
            frameCount--;
            if (frameCount <= 0)
            {
                Pop();
                frameCount = 0;
                stack[0] = result;
                stackTop = stack + 1;
                return INTERPRET_OK;
            }

            stackTop = frame->slots;
            Push(result);

            frame = &frames[frameCount - 1];
            break;
        }

        case OP_END:
        {
            return INTERPRET_OK;
        }

        case OP_END_MODULE:
            currentModule = frames[frameCount - 1].closure->function->module;
            // Push(NIL_VAL);
            break;

        case OP_CONST:
        {
            Value constant = READ_CONSTANT();
            Push(constant);
            break;
        }

        case OP_SUBSCRIPT:
        {
            Value indexValue = Peek(0);
            Value subscriptValue = Peek(1);

            if (!IS_OBJ(subscriptValue))
            {
                frame->ip = ip;
                RuntimeError("Can only subscript on lists, strings or dictionaries.");
                return INTERPRET_RUNTIME_ERROR;
            }

            switch (AS_OBJ(subscriptValue)->type)
            {
                case OBJ_ARRAY:
                {
                    if (!IS_NUMBER(indexValue) && !IS_INT(indexValue))
                    {
                        RuntimeError("List index must be a number.");
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    ObjectArray *list = AS_ARRAY(subscriptValue);
                    int index = AS_INT(indexValue);

                    // Allow negative indexes
                    if (index < 0)
                        index = list->m_vValues.size() + index;

                    if (index >= 0 && index < list->m_vValues.size())
                    {
                        Pop();
                        Pop();
                        Push(list->m_vValues[index]);
                        break;
                    }

                    RuntimeError("List index out of bounds.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                case OBJ_STRING:
                {
                    ObjectString *string = AS_STRING(subscriptValue);
                    int index = AS_INT(indexValue);

                    // Allow negative indexes
                    if (index < 0)
                        index = string->string.size() + index;

                    if (index >= 0 && index < string->string.size()) {
                        Pop();
                        Pop();
                        Push(OBJ_VAL(m_oParser.CopyString(std::string(1, string->string[index]))));
                        break;
                    }

                    RuntimeError("String index out of bounds.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                // case OBJ_DICT: {
                //     ObjDict *dict = AS_DICT(subscriptValue);
                //     if (!isValidKey(indexValue)) {
                //         frame->ip = ip;
                //         runtimeError(vm, "Dictionary key must be an immutable type.");
                //         return INTERPRET_RUNTIME_ERROR;
                //     }

                //     Value v;
                //     pop(vm);
                //     pop(vm);
                //     if (dictGet(dict, indexValue, &v)) {
                //         push(vm, v);
                //     } else {
                //         push(vm, NIL_VAL);
                //     }

                //     break;
                // }

                default:
                {
                    frame->ip = ip;
                    RuntimeError("Can only subscript on lists, strings or dictionaries.");
                    return INTERPRET_RUNTIME_ERROR;
                }
            }
            break;
        }

        case OP_FOREIGN:
        {
            if (IS_OBJ(Peek(0)))
            {
                AS_OBJ(Peek(0))->bIsForeign = true;
            }
            break;
        }

        case OP_ARRAY:
        {
            ObjectArray *array = gc.New<ObjectArray>();
            Push(OBJ_VAL(array));
            break;
        }

        case OP_ADD_LIST:
        {
            int argCount = READ_BYTE();
            Value listValue = Peek(argCount);

            ObjectArray *array = AS_ARRAY(listValue);

            for (int i = argCount - 1; i >= 0; i--)
            {
                array->m_vValues.push_back(Peek(i));
            }

            stackTop -= argCount;

            Pop();

            Push(OBJ_VAL(array));
            break;
        }
        }
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

    // AddTableToRoot(globals);
    AddTableToRoot(modules);
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
    gc.AddRoot(object);

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

void VM::DefineMethod(ObjectString *name)
{
    Value method = Peek(0);
    ObjectClass *klass = AS_CLASS(Peek(1));
    klass->methods.Set(name, method);
    Pop();
}

bool VM::BindMethod(ObjectClass *klass, ObjectString *name)
{
    Value method;
    if (!klass->methods.Get(name, method)) {
        RuntimeError("Undefined property '%s'.", name->string.c_str());
        return false;
    }

    ObjectBoundMethod *bound = gc.New<ObjectBoundMethod>(Peek(0), AS_CLOSURE(method));
    Pop();
    Push(OBJ_VAL(bound));
    return true;
}


// USER!!
Value VM::FindVariable(ObjectModule* module, const char* name)
{
    Value oValue;
    if (module->m_vVariables.Get(m_oParser.CopyString(name), oValue))
        return oValue;
    return NIL_VAL;
}

void VM::GetVariable(const char* module, const char* name, int slot)
{
    FOX_ASSERT(module != NULL, "Module cannot be NULL.");
    FOX_ASSERT(name != NULL, "Variable name cannot be NULL.");  

    Value moduleName = NewString(module);
    Push(moduleName);
    
    ObjectModule* moduleObj = GetModule(moduleName);
    FOX_ASSERT(moduleObj != NULL, "Could not find module.");
    
    Pop(); // moduleName.

    Value oVariable = FindVariable(moduleObj, name);
    FOX_ASSERT(!(oVariable == NIL_VAL), "Could not find variable.");
    
    SetSlot(slot, oVariable);
}

// Looks up the previously loaded module with [name].
//
// Returns `NULL` if no module with that name has been loaded.
ObjectModule* VM::GetModule(Value name)
{
    Value moduleValue;
    if (IS_STRING(name) && modules.Get(AS_STRING(name), moduleValue))
        return AS_MODULE(moduleValue);
    return NULL;
}

ObjectClosure* VM::CompileInModule(Value name, const char* source, bool isExpression, bool printErrors)
{
    // See if the module has already been loaded.
    ObjectModule* module = GetModule(name);
    if (module == NULL)
    {
        module = gc.New<ObjectModule>(AS_STRING(name));

        // It's possible for the wrenMapSet below to resize the modules map,
        // and trigger a GC while doing so. When this happens it will collect
        // the module we've just created. Once in the map it is safe.
        Push(OBJ_VAL(module));

        // Store it in the VM's module registry so we don't load the same module
        // multiple times.
        modules.Set(AS_STRING(name), OBJ_VAL(module));

        Pop();

        // Implicitly import the core module.
        ObjectModule* coreModule = GetModule(NewString("core"));
        for (int i = 0; i < coreModule->m_vVariables.m_iCount; i++)
        {
            module->m_vVariables.AddAll(coreModule->m_vVariables);
        }
    }

    currentModule = module;

    Chunk chunk;
    ObjectFunction* fn = Compile(m_oParser, source, &chunk);
    if (fn == NULL)
    {
        // TODO: Should we still store the module even if it didn't compile?
        return NULL;
    }

    // Functions are always wrapped in closures.
    Push(OBJ_VAL(fn));
    ObjectClosure* closure = gc.New<ObjectClosure>(this, fn);
    Pop(); // fn.

    return closure;
}


// // Let the host resolve an imported module name if it wants to.
// Value ResolveModule(VM* vm, Value name)
// {
//   // If the host doesn't care to resolve, leave the name alone.
//   // if (vm->config.resolveModuleFn == NULL) return name;

//   // ObjFiber* fiber = vm->fiber;
// //   ObjFn* fn = fiber->frames[fiber->numFrames - 1].closure->fn;
// //   ObjString* importer = fn->module->name;
//     ObjectFunction* fn = vm->frames[vm->frameCount - 1].closure->function;
//     ObjectString* importer = fn->module->name;
  
//   const char* resolved = vm->config.resolveModuleFn(vm, importer->value,
//                                                     AS_CSTRING(name));
//   if (resolved == NULL)
//   {
//     vm->fiber->error = wrenStringFormat(vm,
//         "Could not resolve module '@' imported from '@'.",
//         name, OBJ_VAL(importer));
//     return NULL_VAL;
//   }
  
//   // If they resolved to the exact same string, we don't need to copy it.
//   if (resolved == AS_CSTRING(name)) return name;

//   // Copy the string into a Wren String object.
//   name = wrenNewString(vm, resolved);
//   DEALLOCATE(vm, (char*)resolved);
//   return name;
// }

Value VM::ImportModule(Value name)
{
    // name = resolveModule(vm, name);
    
    // If the module is already loaded, we don't need to do anything.
    Value existing;
    if (modules.Get(AS_STRING(name), existing))
        return existing;
    
    Push(name);

    const char* source = NULL;
    bool allocatedSource = true;
    
    // Let the host try to provide the module.
    // if (vm->config.loadModuleFn != NULL)
    // {
    //     source = vm->config.loadModuleFn(vm, AS_CSTRING(name));
    // }
    
    // If the host didn't provide it, see if it's a built in optional module.
    // if (source == NULL)
    // {
    ObjectString* nameString = AS_STRING(name);
    nameString->string += ".fox";

    std::ifstream t(nameString->string);
    std::string str((   std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
    source = str.c_str();
    allocatedSource = false;
    // }
    
    if (source == NULL)
    {
        RuntimeError("Could not load module '%s'.", AS_CSTRING(name));
        Pop(); // name.
        return NIL_VAL;
    }
    
    ObjectClosure* moduleClosure = CompileInModule(name, source, false, true);
    
    // Modules loaded by the host are expected to be dynamically allocated with
    // ownership given to the VM, which will free it. The built in optional
    // modules are constant strings which don't need to be freed.
    // if (allocatedSource) DEALLOCATE(vm, (char*)source);
    
    if (moduleClosure == NULL)
    {
        RuntimeError("Could not compile module '%s'.", AS_CSTRING(name));
        Pop(); // name.
        return NIL_VAL;
    }

    Pop(); // name.

    // Return the closure that executes the module.
    return OBJ_VAL(moduleClosure);
}

Value VM::GetModuleVariable(ObjectModule* module, Value variableName)
{
    ObjectString* variable = AS_STRING(variableName);
    Value oModule;

    // It's a runtime error if the imported variable does not exist.
    if (module->m_vVariables.Get(AS_STRING(variableName), oModule))
        return oModule;
    
    RuntimeError("Could not find a variable named '%s' in module '%s'.", AS_CSTRING(variableName), module->m_strName->string.c_str());
    return NIL_VAL;
}

InterpretResult VM::Call(Handle* pMethod)
{
    FOX_ASSERT(pMethod != NULL, "Method cannot be NULL.");
    FOX_ASSERT(IS_CLOSURE(pMethod->value), "Method must be a method handle.");
    FOX_ASSERT(m_pApiStack != NULL, "Must set up arguments for call first.");
    
    ObjectClosure* closure = AS_CLOSURE(pMethod->value);

    FOX_ASSERT(stackTop - m_pApiStack >= closure->function->arity, "Stack must have enough arguments for method.");

    m_pApiStack = NULL;
    // stackTop = &stack[closure->function->arity + 1];
    CallFunction(closure, closure->function->arity);
    InterpretResult result = run();
    m_pApiStack = stack;
    return result;
}

Callable VM::Function(const std::string& strModuleName, const std::string& strSignature)
{
    EnsureSlots(1);
    int nameLength = 0;

    for (int i = 0; i < strSignature.size() && strSignature[i] != '('; i++)
        nameLength++;

    std::string strName = strSignature.substr(0, nameLength);
    GetVariable(strModuleName.c_str(), strName.c_str(), 0);
    Handle* variable = GetSlotHandle(0);
    Pop();
    Handle* handle = MakeCallHandle(strSignature.c_str());

    Callable m;
    m.m_pVariable = variable;
    m.m_pMethod = handle;
    m.m_pVM = this;
    return m;
}

Handle* VM::MakeHandle(Value value)
{
    if (IS_OBJ(value)) Push(value);
    
    // Make a handle for it.
    Handle* handle = new Handle;
    handle->value = value;

    if (IS_OBJ(value)) Pop();

    // Add it to the front of the linked list of handles.
    m_vHandles.push_back(handle);
    
    return handle;
}

Handle* VM::MakeCallHandle(const char* signature)
{
    FOX_ASSERT(signature != NULL, "Signature cannot be NULL.");
    
    int signatureLength = (int)strlen(signature);
    FOX_ASSERT(signatureLength > 0, "Signature cannot be empty.");

    // Count the number parameters the method expects.
    int numParams = 0;
    if (signature[signatureLength - 1] == ')')
    {
        for (int i = signatureLength - 1; i > 0 && signature[i] != '('; i--)
        {
            if (signature[i] == '_')
                numParams++;
        }
    }
    
    // Count subscript arguments.
    if (signature[0] == '[')
    {
        for (int i = 0; i < signatureLength && signature[i] != ']'; i++)
        {
            if (signature[i] == '_')
                numParams++;
        }
    }
    
    // Create a little stub function that assumes the arguments are on the stack
    // and calls the method.
    ObjectFunction* fn = gc.New<ObjectFunction>();
    
    // Wrap the function in a closure and then in a handle. Do this here so it
    // doesn't get collected as we fill it in.
    Handle* value = MakeHandle(OBJ_VAL(fn));
    value->value = OBJ_VAL(gc.New<ObjectClosure>(this, fn));

    fn->chunk.WriteChunk((uint8_t)OP_CALL, 0);
    fn->chunk.WriteChunk((uint8_t)numParams, 0);
    fn->chunk.WriteChunk((uint8_t)OP_RETURN, 0);
    fn->name = AS_STRING(NewString(signature));
    return value;
}

void VM::ReleaseHandle(Handle* handle)
{
    FOX_ASSERT(handle != NULL, "Handle cannot be NULL.");

    handle->value = NIL_VAL;
    std::vector<Handle*>::iterator it = std::find(m_vHandles.begin(), m_vHandles.end(), handle);
    m_vHandles.erase(it);
    delete handle;
}

int VM::GetSlotCount()
{
    if (m_pApiStack == NULL) return 0;
    return (int)(stackTop - m_pApiStack);
}

void VM::EnsureSlots(int numSlots)
{
    for (int i = 0; i < numSlots; i++)
        Push(NIL_VAL);

    m_pApiStack = stackTop - numSlots;
}

// Ensures that [slot] is a valid index into the API's stack of slots.
void VM::ValidateApiSlot(int slot)
{
    FOX_ASSERT(slot >= 0, "Slot cannot be negative.");
    FOX_ASSERT(slot < GetSlotCount(), "Not that many slots.");
}


// Gets the type of the object in [slot].
ValueType VM::GetSlotType(int slot)
{
    ValidateApiSlot(slot);
    if (IS_BOOL(m_pApiStack[slot])) return VAL_BOOL;
    if (IS_NUMBER(m_pApiStack[slot])) return VAL_NUMBER;
//   if (IS_LIST(m_pApiStack[slot])) return WREN_TYPE_LIST;
//   if (IS_MAP(m_pApiStack[slot])) return WREN_TYPE_MAP;
    if (IS_NIL(m_pApiStack[slot])) return VAL_NIL;
//   if (IS_STRING(m_pApiStack[slot])) return WREN_TYPE_STRING;
    return VAL_NIL;
}

Value VM::GetSlot(int slot)
{
    ValidateApiSlot(slot);

    return m_pApiStack[slot];
}

bool VM::GetSlotBool(int slot)
{
    ValidateApiSlot(slot);
    FOX_ASSERT(IS_BOOL(m_pApiStack[slot]), "Slot must hold a bool.");

    return AS_BOOL(m_pApiStack[slot]);
}

double VM::GetSlotDouble(int slot)
{
    ValidateApiSlot(slot);
    FOX_ASSERT(IS_NUMBER(m_pApiStack[slot]), "Slot must hold a number.");

    return AS_NUMBER(m_pApiStack[slot]);
}

const char* VM::GetSlotString(int slot)
{
    ValidateApiSlot(slot);
    FOX_ASSERT(IS_STRING(m_pApiStack[slot]), "Slot must hold a string.");

    return AS_CSTRING(m_pApiStack[slot]);
}

Handle* VM::GetSlotHandle(int slot)
{
    ValidateApiSlot(slot);
    return MakeHandle(m_pApiStack[slot]);
}

// Stores [value] in [slot] in the foreign call stack.
void VM::SetSlot(int slot, Value value)
{
    ValidateApiSlot(slot);
    m_pApiStack[slot] = value;
}

void VM::SetSlotBool(int slot, bool value)
{
    SetSlot(slot, BOOL_VAL(value));
}

void VM::SetSlotDouble(int slot, double value)
{
    SetSlot(slot, NUMBER_VAL(value));
}

void VM::SetSlotInteger(int slot, int value)
{
    SetSlot(slot, INT_VAL(value));
}

void VM::SetSlotNewList(int slot)
{
    SetSlot(slot, OBJ_VAL(gc.New<ObjectArray>()));
}

// void wrenSetSlotNewMap(int slot)
// {
//   setSlot(vm, slot, OBJ_VAL(wrenNewMap(vm)));
// }

void VM::SetSlotNull(int slot)
{
    SetSlot(slot, NIL_VAL);
}

void VM::SetSlotString(int slot, const char* text)
{
    FOX_ASSERT(text != NULL, "String cannot be NULL.\n");
    
    SetSlot(slot, NewString(text));
}

void VM::SetSlotHandle(int slot, Handle* handle)
{
    FOX_ASSERT(handle != NULL, "Handle cannot be NULL.");

    SetSlot(slot, handle->value);
}

int VM::GetListCount(int slot)
{
    ValidateApiSlot(slot);
    FOX_ASSERT(IS_ARRAY(m_pApiStack[slot]), "Slot must hold a list.\n");

    return AS_ARRAY(m_pApiStack[slot])->m_vValues.size();
}

void VM::GetListElement(int listSlot, int index, int elementSlot)
{
    ValidateApiSlot(listSlot);
    ValidateApiSlot(elementSlot);
    FOX_ASSERT(IS_ARRAY(m_pApiStack[listSlot]), "Slot must hold a list.\n");
    
    m_pApiStack[elementSlot] = AS_ARRAY(m_pApiStack[listSlot])->m_vValues[index];
}

void VM::SetListElement(int listSlot, int index, int elementSlot)
{
    ValidateApiSlot(listSlot);
    ValidateApiSlot(elementSlot);
    FOX_ASSERT(IS_ARRAY(m_pApiStack[listSlot]), "Must insert into a list.\n");
    
    ObjectArray* array = AS_ARRAY(m_pApiStack[listSlot]);
    
    // Negative indices count from the end.
    if (index < 0)
        index = array->m_vValues.size() + 1 + index;
    
    FOX_ASSERT(index <= array->m_vValues.size(), "Index out of bounds.\n");

    array->m_vValues[index] = m_pApiStack[elementSlot];
}