#include <chrono>
#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include <fstream>
#include <streambuf>
#include <cstring>

#include "common.h"
#include "debug.h"
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

void VM::Push(Value oValue)
{
    *stackTop = oValue;
    stackTop++;
}

Value VM::Pop()
{
    FOX_ASSERT(stackTop != NULL, "No temporary roots to release.");
    stackTop--;
    return *stackTop;
}

Value VM::Peek(int iDistance) {
    return stackTop[-1 - iDistance];
}

Value VM::PeekStart(int iDistance)
{
    if (!isInit)
        return stack[iDistance];
    return stack[iDistance + 1];
}

bool IsFalsey(Value oValue) {
    return IS_NIL(oValue) || (IS_BOOL(oValue) && !AS_BOOL(oValue));
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

bool VM::CallFunction(ObjectClosure *pClosure, int iArgCount)
{
    // Check the number of args pass to the function call
    if (iArgCount != pClosure->function->arity) {
        RuntimeError(
            "Expected %d arguments but got %d.",
            pClosure->function->arity,
            iArgCount);
        return false;
    }

    if (frameCount == FRAMES_MAX) {
        RuntimeError("Stack overflow.");
        return false;
    }

    CallFrame *pFrame = &frames[frameCount++];
    pFrame->closure = pClosure;
    pFrame->ip = pClosure->function->chunk.m_vCode.begin();

    pFrame->slots = stackTop - iArgCount - 1;
    return true;
}

bool VM::CallValue(Value oCallee, int iArgCount)
{
    if (IS_OBJ(oCallee))
    {
        switch (OBJ_TYPE(oCallee))
        {
        case OBJ_NATIVE:
        {
            NativeFn native = AS_NATIVE(oCallee);
            Value oResult = native(this, iArgCount, stackTop - iArgCount);
            stackTop -= iArgCount + 1;
            Push(oResult);
            return true;
        }

        case OBJ_BOUND_METHOD:
        {
            ObjectBoundMethod *pBound = AS_BOUND_METHOD(oCallee);
            stackTop[-iArgCount - 1] = pBound->receiver;
            return CallFunction(pBound->method, iArgCount);
        }

        case OBJ_CLASS:
        {
            ObjectClass *pKlass = AS_CLASS(oCallee);
            stackTop[-iArgCount - 1] = OBJ_VAL(gc.New<ObjectInstance>(pKlass));
            Value oInitializer;
            if (pKlass->methods.Get(initString, oInitializer))
                return CallValue(oInitializer, iArgCount);
            else if (iArgCount != 0)
            {
                RuntimeError("Expected 0 arguments but got %d.", iArgCount);
                return false;
            }
            return true;
        }

        case OBJ_CLOSURE:
            return CallFunction(AS_CLOSURE(oCallee), iArgCount);

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
    ObjectUpvalue *pPrevUpvalue = NULL;
    ObjectUpvalue *pUpvalue = openUpvalues;

    while (pUpvalue != NULL && pUpvalue->location > local) {
        pPrevUpvalue = pUpvalue;
        pUpvalue = pUpvalue->next;
    }

    if (pUpvalue != NULL && pUpvalue->location == local)
        return pUpvalue;
    ObjectUpvalue *pCreatedUpvalue = gc.New<ObjectUpvalue>(local);
    if (pPrevUpvalue == NULL) {
        openUpvalues = pCreatedUpvalue;
    } else {
        pPrevUpvalue->next = pCreatedUpvalue;
    }
    return pCreatedUpvalue;
}

void VM::CloseUpvalues(Value *last)
{
    while (openUpvalues != NULL && openUpvalues->location >= last) {
        ObjectUpvalue *pUpvalue = openUpvalues;
        pUpvalue->closed = *pUpvalue->location;
        pUpvalue->location = &pUpvalue->closed;
        openUpvalues = pUpvalue->next;
    }
}

void VM::DefineFunction(const std::string &strModule, const std::string &strName, NativeFn function)
{
    Value oStrModuleName = NewString(strModule.c_str());

    // See if the module has already been loaded.
    ObjectModule* pModule = GetModule(oStrModuleName);
    if (pModule != NULL)
    {
        Push(NewString(strName.c_str()));
        Push(OBJ_VAL(gc.New<ObjectNative>(function)));
        pModule->m_vVariables.Set(AS_STRING(PeekStart(0)), PeekStart(1));
        Pop();
        Pop();
    }
    else
        std::cerr << "'" << strModule << "': Could not find the module." << std::endl;
}

void VM::DefineLib(const std::string &strModule, const std::string &strName, NativeMethods &functions)
{
    Value oStrModuleName = NewString(strModule.c_str());

    // See if the module has already been loaded.
    ObjectModule* pModule = GetModule(oStrModuleName);
    if (pModule != NULL)
    {
        Push(OBJ_VAL(m_oParser.CopyString(strName)));
        Push(OBJ_VAL(gc.New<ObjectLib>(AS_STRING(PeekStart(0)))));
        pModule->m_vVariables.Set(AS_STRING(PeekStart(0)), PeekStart(1));
        ObjectLib *pKlass = AS_LIB(Pop());
        Pop();
        Push(OBJ_VAL(pKlass));
        for (auto &it : functions)
        {
            Push(OBJ_VAL(m_oParser.CopyString(it.first)));
            Push(OBJ_VAL(gc.New<ObjectNative>(it.second)));

            pKlass->methods.Set(AS_STRING(PeekStart(1)), PeekStart(2));

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
    ObjectModule* pModule = GetModule(oStrName);
    if (pModule == NULL)
    {
        pModule = gc.New<ObjectModule>(AS_STRING(oStrName));

        // It's possible for the wrenMapSet below to resize the modules map,
        // and trigger a GC while doing so. When this happens it will collect
        // the module we've just created. Once in the map it is safe.
        Push(OBJ_VAL(pModule));

        // Store it in the VM's module registry so we don't load the same module
        // multiple times.
        modules.Set(AS_STRING(oStrName), OBJ_VAL(pModule));

        Pop();

        // Implicitly import the core module.
    //     ObjectModule* coreModule = GetModule(NewString("core"));
    //     for (int i = 0; i < coreModule->m_vVariables.m_iCount; i++)
    //         module->m_vVariables.AddAll(coreModule->m_vVariables);
    }
    else
        std::cerr << "'" << strName << "': This module already exist !!" << std::endl;
}

void VM::DefineClass(const std::string &strModule, const std::string &strName, NativeMethods &functions)
{
    Value oStrModuleName = NewString(strModule.c_str());

    // See if the module has already been loaded.
    ObjectModule* pModule = GetModule(oStrModuleName);
    if (pModule != NULL)
    {
        Push(OBJ_VAL(m_oParser.CopyString(strName)));
        Push(OBJ_VAL(gc.New<ObjectClass>(AS_STRING(PeekStart(0)))));
        pModule->m_vVariables.Set(AS_STRING(PeekStart(0)), PeekStart(1));
        ObjectClass *pKlass = AS_CLASS(Pop());
        Pop();
        Push(OBJ_VAL(pKlass));
        for (auto &it : functions) {
            NativeFn func = it.second;

            Push(OBJ_VAL(m_oParser.CopyString(it.first)));
            Push(OBJ_VAL(gc.New<ObjectNative>(func)));

            pKlass->methods.Set(AS_STRING(PeekStart(1)), PeekStart(2));

            Pop();
            Pop();
        }
        Pop();
    }
}

void VM::DefineBuiltIn(Table& methods, NativeMethods& functions)
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

bool VM::InvokeFromClass(ObjectClass *pKlass, ObjectString *pName, int iArgCount)
{
    Value oMethod;
    if (!pKlass->methods.Get(pName, oMethod))
    {
        RuntimeError("Undefined property '%s'.", pName->string.c_str());
        return false;
    }

    if (oMethod == NIL_VAL)
    {
        RuntimeError("The class '%s' doesn't implement interface members '%s'.", pKlass->name->string.c_str(), pName->string.c_str());
        return false;
    }

    return CallValue(oMethod, iArgCount);
}

bool VM::Invoke(ObjectString *pName, int iArgCount)
{
    Value oReceiver = Peek(iArgCount);
    Value oValue;

    switch (AS_OBJ(oReceiver)->type)
    {
        case OBJ_INSTANCE:
        {
            ObjectInstance *pInstance = AS_INSTANCE(oReceiver);
            if (pInstance->fields.Get(pName, oValue)) {
                stackTop[-iArgCount - 1] = oValue;
                return CallValue(oValue, iArgCount);
            }
            return InvokeFromClass(pInstance->klass, pName, iArgCount);
        }

        case OBJ_LIB:
        {
            ObjectLib *pInstance = AS_LIB(oReceiver);
            Value method;
            if (!pInstance->methods.Get(pName, method))
            {
                RuntimeError("Undefined property '%s'.", pName->string.c_str());
                return false;
            }
            return CallValue(method, iArgCount);
        }

        case OBJ_ARRAY:
        {
            Value oMethod;
            if (!arrayMethods.Get(pName, oMethod))
            {
                RuntimeError("Undefined methods '%s'.", pName->string.c_str());
                return false;
            }
            return CallValue(oMethod, iArgCount);
        }
        default:
            RuntimeError("Only instances && module have methods.");
            return false;
    }
}

InterpretResult VM::Interpret(const char* strModule, const char* strSource)
{
    ResetStack();
    
    result = INTERPRET_OK;

    ObjectClosure* pClosure = CompileSource(strModule, strSource, false, true);
    if (pClosure == NULL)
        return INTERPRET_COMPILE_ERROR;
    
    Push(OBJ_VAL(pClosure));
    // ObjFiber* fiber = wrenNewFiber(vm, closure);
    // wrenPopRoot(vm); // closure.
    // m_pApiStack = NULL;
    CallValue(OBJ_VAL(pClosure), 0);

    return run();
}

ObjectClosure* VM::CompileSource(const char* strModule, const char* strSource, bool bIsExpression, bool bPrintErrors)
{
    Value oNameValue = NewString("core");
    if (strModule != NULL)
    {
        oNameValue = NewString(strModule);
        Push(oNameValue);
    }
    
    ObjectClosure* pClosure = CompileInModule(oNameValue, strSource, bIsExpression, bPrintErrors);

    if (strModule != NULL) Pop(); // oNameValue.
    return pClosure;
}


Value VM::NewString(const char* strString)
{
    return OBJ_VAL(m_oParser.TakeString(strString));
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
        for (Value *pSlot = stack; pSlot < stackTop; pSlot++) {
            printf("[ ");
            PrintValue(*pSlot);
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
            uint8_t uSlot = READ_BYTE();
            Push(*frame->closure->upValues[uSlot]->location);
            break;
        }

        case OP_SET_UPVALUE: {
            uint8_t uSlot = READ_BYTE();
            *frame->closure->upValues[uSlot]->location = Peek(0);
            break;
        }

        case OP_GET_LOCAL: {
            uint8_t uSlot = READ_BYTE();
            Push(frame->slots[uSlot]);
            break;
        }

        case OP_SET_LOCAL:
        {
            uint8_t uSlot = READ_BYTE();
            frame->slots[uSlot] = Peek(0);
            break;
        }

        case OP_GET_GLOBAL:
        {
            ObjectString *pName = READ_STRING();
            Value oValue;
            if (!currentModule->m_vVariables.Get(pName, oValue)) {
                RuntimeError("Undefined variable '%s'.", pName->string.c_str());
                return INTERPRET_RUNTIME_ERROR;
            }
            Push(oValue);
            break;
        }

        case OP_DEFINE_GLOBAL: {
            ObjectString *pName = READ_STRING();
            currentModule->m_vVariables.Set(pName, Peek(0));
            Pop();
            break;
        }

        case OP_SET_GLOBAL: {
            ObjectString *pName = READ_STRING();
            if (currentModule->m_vVariables.Set(pName, Peek(0))) {
                currentModule->m_vVariables.Delete(pName);
                RuntimeError("Undefined variable '%s'.", pName->string.c_str());
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }

        case OP_GET_PROPERTY: {
            if (!IS_INSTANCE(Peek(0))) {
                RuntimeError("Only instances have properties.");
                return INTERPRET_RUNTIME_ERROR;
            }

            ObjectInstance *pInstance = AS_INSTANCE(Peek(0));
            ObjectString *pName = READ_STRING();

            Value value;
            if (pInstance->cStruct != NULL)
            {
                Value oGetterFunc;
                if (pInstance->getters.Get(pName, oGetterFunc)) {
                    CallValue(oGetterFunc, 0);
                }
                break;
            }
            if (pInstance->fields.Get(pName, value)) {
                Pop(); // Instance.
                Push(value);
                break;
            }

            if (!BindMethod(pInstance->klass, pName)) {
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

            ObjectInstance *pInstance = AS_INSTANCE(Peek(1));
            if (pInstance->cStruct != NULL)
            {
                Value oSetterFunc;
                if (pInstance->setters.Get(READ_STRING(), oSetterFunc)) {
                    CallValue(oSetterFunc, 1);
                }
            }
            else
            {
                pInstance->fields.Set(READ_STRING(), Peek(0));
                Value oValue = Pop();
                Pop();
                Push(oValue);
            }
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
            int iArgCount = READ_BYTE();
            int iTempArgCount = iArgCount;
            int iPercentCount = 0;
            Value string = Peek(--iTempArgCount);
            
            for (int i = 0; AS_STRING(string)->string[i]; i++)
            {
                if (AS_STRING(string)->string[i] == '%' && AS_STRING(string)->string[i + 1] == '%')
                    i++;
                else if (AS_STRING(string)->string[i] == '%')
                    iPercentCount++;
            }
            
            if (iTempArgCount != iPercentCount)
            {
                RuntimeError("Expected %d arguments but got %d in print call.", iPercentCount, iTempArgCount);
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
                    PrintValue(Peek(--iTempArgCount));
            }
            stackTop -= iArgCount;
            break;
        }

        case OP_PRINT_REPL:
        {
            PrintValue(Pop());
            break;
        }

        case OP_JUMP: {
            uint16_t uOffset = READ_SHORT();
            frame->ip += uOffset;
            break;
        }

        case OP_JUMP_IF_FALSE: {
            uint16_t uOffset = READ_SHORT();
            if (IsFalsey(Peek(0)))
                frame->ip += uOffset;
            break;
        }

        case OP_LOOP: {
            uint16_t uOffset = READ_SHORT();
            frame->ip -= uOffset;
            break;
        }

        case OP_CALL: {
            int iArgCount = READ_BYTE();
            if (!CallValue(Peek(iArgCount), iArgCount))
                return INTERPRET_RUNTIME_ERROR;
            frame = &frames[frameCount - 1];
            break;
        }

        case OP_CLASS:
            Push(OBJ_VAL(gc.New<ObjectClass>(READ_STRING())));
            break;
        
        case OP_INHERIT: {
            Value oSuperclass = Peek(1);

            if (!IS_CLASS(oSuperclass)) {
                RuntimeError("Superclass must be a class.");
                return INTERPRET_RUNTIME_ERROR;
            }

            ObjectClass *pSubclass = AS_CLASS(Peek(0));

            pSubclass->methods.AddAll(AS_CLASS(oSuperclass)->methods);
            pSubclass->superClass = AS_CLASS(oSuperclass);
            pSubclass->derivedCount = AS_CLASS(oSuperclass)->derivedCount + 1;
            Pop(); // Subclass.
            break;
        }

        case OP_METHOD:
            DefineMethod(READ_STRING());
            break;

        case OP_INVOKE: {
            ObjectString *pMethod = READ_STRING();
            int iArgCount = READ_BYTE();
            if (!Invoke(pMethod, iArgCount)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            frame = &frames[frameCount - 1];
            break;
        }

        case OP_SUPER_INVOKE: {
            ObjectString *pMethod = READ_STRING();
            int iArgCount = READ_BYTE();
            ObjectClass *pSuperclass = AS_CLASS(Pop());
            if (!InvokeFromClass(pSuperclass, pMethod, iArgCount)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            frame = &frames[frameCount - 1];
            break;
        }

        case OP_CLOSURE: {
            ObjectFunction *pFunction = AS_FUNCTION(READ_CONSTANT());
            ObjectClosure *pClosure = gc.New<ObjectClosure>(this, pFunction);
            Push(OBJ_VAL(pClosure));

            for (int i = 0; i < pClosure->upvalueCount; i++) {
                uint8_t uIsLocal = READ_BYTE();
                uint8_t uIndex = READ_BYTE();
                if (uIsLocal)
                    pClosure->upValues[i] = CaptureUpvalue(frame->slots + uIndex);
                else
                    pClosure->upValues[i] = frame->closure->upValues[uIndex];
            }
            break;
        }

        case OP_CLOSE_UPVALUE:
            CloseUpvalues(stackTop - 1);
            Pop();
            break;

        case OP_GET_SUPER: {
            ObjectString *pName = READ_STRING();
            ObjectClass *pSuperclass = AS_CLASS(Pop());
            if (!BindMethod(pSuperclass, pName)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }

        case OP_IMPORT:
        {
            Push(ImportModule(OBJ_VAL(READ_STRING())));

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
            }
            break;
        }

        case OP_RETURN: {
            Value oResult = Pop();
            CloseUpvalues(frame->slots);
            frameCount--;
            if (frameCount <= 0)
            {
                Pop();
                frameCount = 0;
                stack[0] = oResult;
                stackTop = stack + 1;
                return INTERPRET_OK;
            }

            stackTop = frame->slots;
            Push(oResult);

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
            Value oConstant = READ_CONSTANT();
            Push(oConstant);
            break;
        }

        case OP_SUBSCRIPT:
        {
            Value oIndexValue = Peek(0);
            Value oSubscriptValue = Peek(1);

            if (!IS_OBJ(oSubscriptValue))
            {
                frame->ip = ip;
                RuntimeError("Can only subscript on lists, strings or maps.");
                return INTERPRET_RUNTIME_ERROR;
            }

            switch (AS_OBJ(oSubscriptValue)->type)
            {
                case OBJ_ARRAY:
                {
                    if (!IS_NUMBER(oIndexValue) && !IS_INT(oIndexValue))
                    {
                        RuntimeError("List index must be a number.");
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    ObjectArray *pList = AS_ARRAY(oSubscriptValue);
                    int iIndex = AS_INT(oIndexValue);

                    // Allow negative indexes
                    if (iIndex < 0)
                        iIndex = pList->m_vValues.size() + iIndex;

                    if (iIndex >= 0 && iIndex < pList->m_vValues.size())
                    {
                        Pop();
                        Pop();
                        Push(pList->m_vValues[iIndex]);
                        break;
                    }

                    RuntimeError("List index out of bounds.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                case OBJ_STRING:
                {
                    ObjectString *pString = AS_STRING(oSubscriptValue);
                    int iIndex = AS_INT(oIndexValue);

                    // Allow negative indexes
                    if (iIndex < 0)
                        iIndex = pString->string.size() + iIndex;

                    if (iIndex >= 0 && iIndex < pString->string.size()) {
                        Pop();
                        Pop();
                        Push(OBJ_VAL(m_oParser.CopyString(std::string(1, pString->string[iIndex]))));
                        break;
                    }

                    RuntimeError("String index out of bounds.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                case OBJ_MAP: {
                    ObjectMap *pMap = AS_MAP(oSubscriptValue);
                    // if (!isValidKey(oIndexValue)) {
                    //     frame->ip = ip;
                    //     RuntimeError("Map key must be an immutable type.");
                    //     return INTERPRET_RUNTIME_ERROR;
                    // }

                    Value oValue;
                    Pop();
                    Pop();
                    if (pMap->m_vValues.Get(oIndexValue, oValue))
                        Push(oValue);
                    else
                        Push(NIL_VAL);
                    break;
                }

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
            Push(OBJ_VAL(gc.New<ObjectArray>()));
            break;
        }

        case OP_MAP:
        {
            Push(OBJ_VAL(gc.New<ObjectMap>()));
            break;
        }

        case OP_ADD_LIST:
        {
            int iArgCount = READ_BYTE();
            Value oListValue = Peek(iArgCount);

            ObjectArray *pArray = AS_ARRAY(oListValue);

            for (int i = iArgCount - 1; i >= 0; i--)
            {
                pArray->m_vValues.push_back(Peek(i));
            }

            stackTop -= iArgCount;

            Pop();

            Push(OBJ_VAL(pArray));
            break;
        }

        case OP_ADD_MAP:
        {
            int iArgCount = READ_BYTE();
            iArgCount *= 2;
            Value oMapValue = Peek(iArgCount);

            ObjectMap *pMap = AS_MAP(oMapValue);

            for (int i = iArgCount - 1; i >= 0; i -= 2)
            {
                pMap->m_vValues.Set(Peek(i), Peek(i - 1));
            }

            stackTop -= iArgCount;

            Pop();

            Push(OBJ_VAL(pMap));
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

void VM::AddToRoots()
{
    for (Value *slot = stack; slot < stackTop; slot++)
        AddValueToRoot(*slot);

    for (int i = 0; i < frameCount; i++)
        AddObjectToRoot(frames[i].closure);

    for (ObjectUpvalue *upvalue = openUpvalues; upvalue != NULL; upvalue = upvalue->next)
        AddObjectToRoot(upvalue);

    for (auto& handle : m_vHandles)
    {
        AddObjectToRoot(handle);
        AddValueToRoot(handle->value);
        PrintValue(handle->value);
    }
    
    AddTableToRoot(modules);
    AddTableToRoot(arrayMethods);
    AddCompilerToRoots();
    AddObjectToRoot(initString);
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

void VM::DefineVariable(const char* strModule, const char* strName, Value oValue)
{
    FOX_ASSERT(strModule != NULL, "Module cannot be NULL.");
    FOX_ASSERT(strName != NULL, "Variable name cannot be NULL.");

    // See if the module has already been loaded.
    ObjectModule* pModule = GetModule(NewString(strModule));
    FOX_ASSERT(pModule != NULL, "Could not find module.");

    if (pModule != NULL)
    {
        pModule->m_vVariables.Set(AS_STRING(NewString(strName)), oValue);
    }
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
    Handle* handle = gc.New<Handle>();
    handle->value = value;

    if (IS_OBJ(value)) Pop();

    // Add it to the front of the linked list of handles.
    m_vHandles.push_back(handle);
    
    return handle;
}

Handle* VM::MakeCallHandle(const char* signature)
{
    FOX_ASSERT(signature != NULL, "Signature cannot be NULL.");
    
    int signatureLength = std::strlen(signature);
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