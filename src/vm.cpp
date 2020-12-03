#include <chrono>
#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include <fstream>
#include <streambuf>
#include <cstring>

#include "common.h"
#include "gc.hpp"
#include "debug.h"
#include "library/library.h"
#include "foxely.h"
#include "Parser.h"
#include "vm.hpp"
#include "object.hpp"
#include "Table.hpp"

VM::VM() : gc(this), m_oParser(this), modules()
{
    // ResetStack();
    m_pCurrentFiber = nullptr;
    isInit = false;
    currentModule = nullptr;
    m_pApiStack = nullptr;
    m_pCurrentFiber = gc.New<ObjectFiber>(nullptr);
    DefineModule("core");
    initString = Fox_AsString(NewString("init"));
    DefineCoreArray(this);
    DefineCoreString(this);
    DefineCoreMap(this);
    DefineCoreFiber(this);
}

VM::~VM()
{
    initString = NULL;
}

void VM::ResetStack()
{
    m_pCurrentFiber->m_pStackTop = m_pCurrentFiber->m_vStack;
    m_pCurrentFiber->m_iFrameCount = 0;
}

void VM::Push(Value oValue)
{
    *m_pCurrentFiber->m_pStackTop = oValue;
    m_pCurrentFiber->m_pStackTop++;
}

Value VM::Pop()
{
    FOX_ASSERT(m_pCurrentFiber->m_pStackTop != NULL, "No temporary roots to release.");
    m_pCurrentFiber->m_pStackTop--;
    return *m_pCurrentFiber->m_pStackTop;
}

Value VM::Peek(int iDistance)
{
    return m_pCurrentFiber->m_pStackTop[-1 - iDistance];
}

Value VM::PeekStart(int iDistance)
{
    if (!isInit)
        return m_pCurrentFiber->m_vStack[iDistance];
    return m_pCurrentFiber->m_vStack[iDistance + 1];
}

bool IsFalsey(Value oValue) {
    return Fox_IsNil(oValue) || (Fox_IsBool(oValue) && !Fox_AsBool(oValue));
}

void VM::RuntimeError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    for (int i = m_pCurrentFiber->m_iFrameCount - 1; i >= 0; i--)
    {
        CallFrame *frame = &m_pCurrentFiber->m_vFrames[i];
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

    if (m_pCurrentFiber->m_iFrameCount == FRAMES_MAX) {
        RuntimeError("Stack overflow.");
        return false;
    }

    CallFrame *pFrame = &m_pCurrentFiber->m_vFrames[m_pCurrentFiber->m_iFrameCount++];
    pFrame->closure = pClosure;
    pFrame->ip = pClosure->function->chunk.m_vCode.begin();

    pFrame->slots = m_pCurrentFiber->m_pStackTop - iArgCount - 1;
    return true;
}

bool VM::CallValue(Value oCallee, int iArgCount)
{
    if (Fox_IsObject(oCallee))
    {
        switch (Fox_ObjectType(oCallee))
        {
        case OBJ_NATIVE:
        {
            NativeFn native = Fox_AsNative(oCallee);
            Value oResult = native(this, iArgCount, m_pCurrentFiber->m_pStackTop - iArgCount);
            m_pCurrentFiber->m_pStackTop -= iArgCount + 1;
            Push(oResult);
            return true;
        }

        case OBJ_BOUND_METHOD:
        {
            ObjectBoundMethod *pBound = Fox_AsBoundMethod(oCallee);
            m_pCurrentFiber->m_pStackTop[-iArgCount - 1] = pBound->receiver;
            return CallFunction(pBound->method, iArgCount);
        }

        case OBJ_CLASS:
        {
            ObjectClass *pKlass = Fox_AsClass(oCallee);
            m_pCurrentFiber->m_pStackTop[-iArgCount - 1] = Fox_Object(gc.New<ObjectInstance>(pKlass));
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
            return CallFunction(Fox_AsClosure(oCallee), iArgCount);

        default:
            // Non-callable object type.
            PrintValue(oCallee);
            break;
        }
    }

    RuntimeError("Can only call functions and classes.");
    return false;
}

ObjectUpvalue *VM::CaptureUpvalue(Value *local)
{
    ObjectUpvalue *pPrevUpvalue = NULL;
    ObjectUpvalue *pUpvalue = m_pCurrentFiber->m_vOpenUpvalues;

    while (pUpvalue != NULL && pUpvalue->location > local) {
        pPrevUpvalue = pUpvalue;
        pUpvalue = pUpvalue->next;
    }

    if (pUpvalue != NULL && pUpvalue->location == local)
        return pUpvalue;
    ObjectUpvalue *pCreatedUpvalue = gc.New<ObjectUpvalue>(local);
    if (pPrevUpvalue == NULL) {
        m_pCurrentFiber->m_vOpenUpvalues = pCreatedUpvalue;
    } else {
        pPrevUpvalue->next = pCreatedUpvalue;
    }
    return pCreatedUpvalue;
}

void VM::CloseUpvalues(Value *last)
{
    while (m_pCurrentFiber->m_vOpenUpvalues != NULL && m_pCurrentFiber->m_vOpenUpvalues->location >= last) {
        ObjectUpvalue *pUpvalue = m_pCurrentFiber->m_vOpenUpvalues;
        pUpvalue->closed = *pUpvalue->location;
        pUpvalue->location = &pUpvalue->closed;
        m_pCurrentFiber->m_vOpenUpvalues = pUpvalue->next;
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
        Push(Fox_Object(gc.New<ObjectNative>(function)));
        pModule->m_vVariables.Set(Fox_AsString(PeekStart(0)), PeekStart(1));
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
        Push(Fox_Object(m_oParser.CopyString(strName)));
        Push(Fox_Object(gc.New<ObjectLib>(Fox_AsString(PeekStart(0)))));
        pModule->m_vVariables.Set(Fox_AsString(PeekStart(0)), PeekStart(1));
        ObjectLib *pKlass = Fox_AsLib(Pop());
        Pop();
        Push(Fox_Object(pKlass));
        for (auto &it : functions)
        {
            Push(Fox_Object(m_oParser.CopyString(it.first)));
            Push(Fox_Object(gc.New<ObjectNative>(it.second)));

            pKlass->methods.Set(Fox_AsString(PeekStart(1)), PeekStart(2));

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
        pModule = gc.New<ObjectModule>(Fox_AsString(oStrName));

        // It's possible for the wrenMapSet below to resize the modules map,
        // and trigger a GC while doing so. When this happens it will collect
        // the module we've just created. Once in the map it is safe.
        Push(Fox_Object(pModule));

        // Store it in the VM's module registry so we don't load the same module
        // multiple times.
        modules.Set(Fox_AsString(oStrName), Fox_Object(pModule));

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
        Push(Fox_Object(m_oParser.CopyString(strName)));
        Push(Fox_Object(gc.New<ObjectClass>(Fox_AsString(PeekStart(0)))));
        pModule->m_vVariables.Set(Fox_AsString(PeekStart(0)), PeekStart(1));
        ObjectClass *pKlass = Fox_AsClass(Pop());
        Pop();
        Push(Fox_Object(pKlass));
        for (auto &it : functions) {
            NativeFn func = it.second;

            Push(Fox_Object(m_oParser.CopyString(it.first)));
            Push(Fox_Object(gc.New<ObjectNative>(func)));

            pKlass->methods.Set(Fox_AsString(PeekStart(1)), PeekStart(2));

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

        Push(Fox_Object(m_oParser.CopyString(it.first)));
        Push(Fox_Object(gc.New<ObjectNative>(func)));

        methods.Set(Fox_AsString(PeekStart(0)), PeekStart(1));

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

    if (oMethod == Fox_Nil)
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

    switch (Fox_AsObject(oReceiver)->type)
    {
        case OBJ_INSTANCE:
        {
            ObjectInstance *pInstance = Fox_AsInstance(oReceiver);
            if (pInstance->fields.Get(pName, oValue)) {
                m_pCurrentFiber->m_pStackTop[-iArgCount - 1] = oValue;
                return CallValue(oValue, iArgCount);
            }
            return InvokeFromClass(pInstance->klass, pName, iArgCount);
        }

        case OBJ_LIB:
        {
            ObjectLib *pInstance = Fox_AsLib(oReceiver);
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

        case OBJ_STRING:
        {
            Value oMethod;
            if (!stringMethods.Get(pName, oMethod))
            {
                RuntimeError("Undefined methods '%s'.", pName->string.c_str());
                return false;
            }
            return CallValue(oMethod, iArgCount);
        }

        case OBJ_MAP:
        {
            Value oMethod;
            if (!mapMethods.Get(pName, oMethod))
            {
                RuntimeError("Undefined methods '%s'.", pName->string.c_str());
                return false;
            }
            return CallValue(oMethod, iArgCount);
        }

        case OBJ_FIBER:
        {
            Value oMethod;
            if (!fiberMethods.Get(pName, oMethod))
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
    // ResetStack();
    
    result = INTERPRET_OK;

    ObjectClosure* pClosure = CompileSource(strModule, strSource, false, true);
    if (pClosure == NULL)
        return INTERPRET_COMPILE_ERROR;
    
    Push(Fox_Object(pClosure));
    // Initialize the first call frame.
    CallFrame *pFrame = &m_pCurrentFiber->m_vFrames[m_pCurrentFiber->m_iFrameCount++];
    pFrame->closure = pClosure;
    pFrame->ip = pClosure->function->chunk.m_vCode.begin();
    
    // The first slot always holds the closure.
    m_pCurrentFiber->m_pStackTop[0] = Fox_Object(pClosure);
    m_pCurrentFiber->m_pStackTop++;
    pFrame->slots = m_pCurrentFiber->m_pStackTop - 1;
    Pop(); // closure.
    // m_pApiStack = NULL;
    // CallValue(Fox_Object(pClosure), 0);

    return run(m_pCurrentFiber);
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
    return Fox_Object(m_oParser.TakeString(strString));
}

static bool ValueIsNumber(Value oNumber)
{
    if (Fox_IsDouble(oNumber))
        return true;
    if (Fox_IsInt(oNumber))
        return true;
    return false;
}

InterpretResult VM::run(ObjectFiber* pFiber)
{
    m_pCurrentFiber = pFiber;
    CallFrame *frame = &m_pCurrentFiber->m_vFrames[m_pCurrentFiber->m_iFrameCount - 1];

#define READ_BYTE() (*frame->ip++)
#define READ_CONSTANT()                                                        \
    (frame->closure->function->chunk.m_oConstants.m_vValues[READ_BYTE()])
#define READ_STRING() Fox_AsString(READ_CONSTANT())
#define READ_SHORT()                                                           \
    (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))

#define BINARY_OP(ValueType, type, op)                                         \
    do {                                                                       \
        if (!ValueIsNumber(Peek(0)) || !ValueIsNumber(Peek(1))) {              \
            RuntimeError("Operands must be numbers.");                         \
            return INTERPRET_RUNTIME_ERROR;                                    \
        }                                                                      \
        type b = Fox_As##ValueType(Pop());                                           \
        type a = Fox_As##ValueType(Pop());                                           \
        Push(Fox_##ValueType(a op b));                                       \
    } while (false)

    while (true) {
        if (result == INTERPRET_RUNTIME_ERROR)
            return result;
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value *pSlot = m_pCurrentFiber->m_vStack; pSlot < m_pCurrentFiber->m_pStackTop; pSlot++) {
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
            Push(Fox_Nil);
            break;
        case OP_TRUE:
            Push(Fox_Bool(true));
            break;
        case OP_FALSE:
            Push(Fox_Bool(false));
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
            if (!Fox_IsInstance(Peek(0))) {
                RuntimeError("Only instances have properties.");
                return INTERPRET_RUNTIME_ERROR;
            }

            ObjectInstance *pInstance = Fox_AsInstance(Peek(0));
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
            if (!Fox_IsInstance(Peek(1)))
            {
                RuntimeError("Only instances have fields.");
                return INTERPRET_RUNTIME_ERROR;
            }

            ObjectInstance *pInstance = Fox_AsInstance(Peek(1));
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
            Push(Fox_Bool(ValuesEqual(a, b)));
            break;
        }

        case OP_GREATER:
            BINARY_OP(Bool, double, >);
            break;
        case OP_LESS:
            BINARY_OP(Bool, double, <);
            break;
        
        case OP_ADD:
        {
            if (Fox_IsString(Peek(0)) && Fox_IsString(Peek(1))) {
                Concatenate();
            } else if (Fox_IsDouble(Peek(0)) && Fox_IsDouble(Peek(1))) {
                double b = Fox_AsDouble(Pop());
                double a = Fox_AsDouble(Pop());
                Push(Fox_Double(a + b));
            } else if (Fox_IsInt(Peek(0)) && Fox_IsInt(Peek(1))) {
                int b = Fox_AsInt(Pop());
                int a = Fox_AsInt(Pop());
                Push(Fox_Int(a + b));
            } else {
                RuntimeError("Operands must be two numbers or two strings.");
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_SUB:
            if (Fox_IsDouble(Peek(0)))
                BINARY_OP(Double, double, -);
            else if (Fox_IsInt(Peek(0)))
                BINARY_OP(Int, int, -);
            break;
        case OP_MUL:
            if (Fox_IsDouble(Peek(0)))
                BINARY_OP(Double, double, *);
            else if (Fox_IsInt(Peek(0)))
                BINARY_OP(Int, int, *);
            break;
        case OP_DIV:
            if (Fox_IsDouble(Peek(0)))
                BINARY_OP(Double, double,  /);
            else if (Fox_IsInt(Peek(0)))
                BINARY_OP(Int, int, /);
            break;
        case OP_NOT:
            Push(Fox_Bool(IsFalsey(Pop())));
            break;
        case OP_NEGATE: {
            if (!Fox_IsDouble(Peek(0))) {
                RuntimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }

            Push(Fox_Double(-Fox_AsDouble(Pop())));
            break;
        }
        case OP_PRINT:
        {
            int iArgCount = READ_BYTE();
            int iTempArgCount = iArgCount;
            int iPercentCount = 0;
            Value string = Peek(--iTempArgCount);
            
            for (int i = 0; Fox_AsString(string)->string[i]; i++)
            {
                if (Fox_AsString(string)->string[i] == '%' && Fox_AsString(string)->string[i + 1] == '%')
                    i++;
                else if (Fox_AsString(string)->string[i] == '%')
                    iPercentCount++;
            }
            
            if (iTempArgCount != iPercentCount)
            {
                RuntimeError("Expected %d arguments but got %d in print call.", iPercentCount, iTempArgCount);
                break;
            }
            
            for (int i = 0; Fox_AsString(string)->string[i]; i++)
            {
                if (Fox_AsString(string)->string[i] != '%') {
                    printf("%c", Fox_AsString(string)->string[i]);
                } else if (Fox_AsString(string)->string[i] == '%' && Fox_AsString(string)->string[i + 1] == '%') {
                    printf("%%");
                    i++;
                } else
                    PrintValue(Peek(--iTempArgCount));
            }
            m_pCurrentFiber->m_pStackTop -= iArgCount;
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
            frame = &m_pCurrentFiber->m_vFrames[m_pCurrentFiber->m_iFrameCount - 1];
            break;
        }

        case OP_CLASS:
            Push(Fox_Object(gc.New<ObjectClass>(READ_STRING())));
            break;
        
        case OP_INHERIT: {
            Value oSuperclass = Peek(1);

            if (!Fox_IsClass(oSuperclass)) {
                RuntimeError("Superclass must be a class.");
                return INTERPRET_RUNTIME_ERROR;
            }

            ObjectClass *pSubclass = Fox_AsClass(Peek(0));

            pSubclass->methods.AddAll(Fox_AsClass(oSuperclass)->methods);
            pSubclass->superClass = Fox_AsClass(oSuperclass);
            pSubclass->derivedCount = Fox_AsClass(oSuperclass)->derivedCount + 1;
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
            frame = &m_pCurrentFiber->m_vFrames[m_pCurrentFiber->m_iFrameCount - 1];
            break;
        }

        case OP_SUPER_INVOKE: {
            ObjectString *pMethod = READ_STRING();
            int iArgCount = READ_BYTE();
            ObjectClass *pSuperclass = Fox_AsClass(Pop());
            if (!InvokeFromClass(pSuperclass, pMethod, iArgCount)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            frame = &m_pCurrentFiber->m_vFrames[m_pCurrentFiber->m_iFrameCount - 1];
            break;
        }

        case OP_CLOSURE: {
            ObjectFunction *pFunction = Fox_AsFunction(READ_CONSTANT());
            ObjectClosure *pClosure = gc.New<ObjectClosure>(this, pFunction);
            Push(Fox_Object(pClosure));

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
            CloseUpvalues(m_pCurrentFiber->m_pStackTop - 1);
            Pop();
            break;

        case OP_GET_SUPER: {
            ObjectString *pName = READ_STRING();
            ObjectClass *pSuperclass = Fox_AsClass(Pop());
            if (!BindMethod(pSuperclass, pName)) {
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }

        case OP_IMPORT:
        {
            Push(ImportModule(Fox_Object(READ_STRING())));

            if (Fox_IsClosure(Peek(0)))
            {
                CallFunction(Fox_AsClosure(Peek(0)), 0);
                frame = &m_pCurrentFiber->m_vFrames[m_pCurrentFiber->m_iFrameCount - 1];
            }
            else
            {
                // The module has already been loaded. Remember it so we can import
                // variables from it if needed.
                currentModule->m_vVariables.AddAll(Fox_AsModule(Pop())->m_vVariables);
                // currentModule = Fox_AsModule(Pop());
            }
            break;
        }

        case OP_RETURN: {
            Value oResult = Pop();
            // Close any upvalues still in scope.
            CloseUpvalues(frame->slots);
            m_pCurrentFiber->m_iFrameCount--;
            // If the fiber is complete, end it.
            if (m_pCurrentFiber->m_iFrameCount <= 0)
            {
                if (m_pCurrentFiber->m_pCaller == nullptr)
                {
            std::cout << "Caller0!!" << std::endl;
                    // Store the final result value at the beginning of the stack so the
                    // C API can get it.
                    Pop();
                    m_pCurrentFiber->m_iFrameCount = 0;
                    m_pCurrentFiber->m_vStack[0] = oResult;
                    m_pCurrentFiber->m_pStackTop = m_pCurrentFiber->m_vStack + 1;
                    return INTERPRET_OK;
                }
            std::cout << "Caller1!!" << std::endl;
                Pop();

                ObjectFiber* pResumingFiber = m_pCurrentFiber->m_pCaller;
                m_pCurrentFiber->m_pCaller = nullptr;
                pFiber = pResumingFiber;
                m_pCurrentFiber = pResumingFiber;
            
                // Store the result in the resuming fiber.
                m_pCurrentFiber->m_pStackTop[-1] = oResult;

                // m_pCurrentFiber->m_pStackTop = frame->slots;
                // Push(oResult);
            }
            else
            {
                Pop();
                std::cout << "Caller2!!" << std::endl;
                // Store the result of the block in the first slot, which is where the
                // caller expects it.
                m_pCurrentFiber->m_vStack[0] = oResult;

                // Discard the stack slots for the call frame (leaving one slot for the
                // result).
                m_pCurrentFiber->m_pStackTop = m_pCurrentFiber->m_vStack + 1;
            }

            frame = &m_pCurrentFiber->m_vFrames[m_pCurrentFiber->m_iFrameCount - 1];
            std::cout << "Caller3!!" << std::endl;
            break;
        }

        case OP_END:
        {
            return INTERPRET_OK;
        }

        case OP_END_MODULE:
            currentModule = m_pCurrentFiber->m_vFrames[m_pCurrentFiber->m_iFrameCount - 1].closure->function->module;
            // Push(Fox_Nil);
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

            if (!Fox_IsObject(oSubscriptValue))
            {
                RuntimeError("Can only subscript on pArrays, strings or maps.");
                return INTERPRET_RUNTIME_ERROR;
            }

            switch (Fox_AsObject(oSubscriptValue)->type)
            {
                case OBJ_ARRAY:
                {
                    if (!Fox_IsDouble(oIndexValue) && !Fox_IsInt(oIndexValue))
                    {
                        RuntimeError("pArray index must be a number.");
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    ObjectArray *ppArray = Fox_AsArray(oSubscriptValue);
                    int iIndex = Fox_AsInt(oIndexValue);

                    // Allow negative indexes
                    if (iIndex < 0)
                        iIndex = ppArray->m_vValues.size() + iIndex;

                    if (iIndex >= 0 && iIndex < ppArray->m_vValues.size())
                    {
                        Pop();
                        Pop();
                        Push(ppArray->m_vValues[iIndex]);
                        break;
                    }

                    RuntimeError("pArray index out of bounds.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                case OBJ_STRING:
                {
                    ObjectString *pString = Fox_AsString(oSubscriptValue);
                    int iIndex = Fox_AsInt(oIndexValue);

                    // Allow negative indexes
                    if (iIndex < 0)
                        iIndex = pString->string.size() + iIndex;

                    if (iIndex >= 0 && iIndex < pString->string.size()) {
                        Pop();
                        Pop();
                        Push(Fox_Object(m_oParser.CopyString(std::string(1, pString->string[iIndex]))));
                        break;
                    }

                    RuntimeError("String index out of bounds.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                case OBJ_MAP: {
                    ObjectMap *pMap = Fox_AsMap(oSubscriptValue);
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
                        Push(Fox_Nil);
                    break;
                }

                default:
                {
                    RuntimeError("Can only subscript on pArrays, strings or dictionaries.");
                    return INTERPRET_RUNTIME_ERROR;
                }
            }
            break;
        }

        case OP_SLICE:
        {
            Value sliceEndIndex = Peek(0);
            Value sliceStartIndex = Peek(1);
            Value objectValue = Peek(2);

            if (!Fox_IsObject(objectValue)) {
                RuntimeError("Can only slice on pArrays and strings.");
            }

            if ((!Fox_IsInt(sliceStartIndex) && !Fox_IsNil(sliceStartIndex)) || (!Fox_IsInt(sliceEndIndex) && !Fox_IsNil(sliceEndIndex))) {
                RuntimeError("Slice index must be a number.");
            }

            int indexStart;
            int indexEnd;
            Value returnVal;

            if (Fox_IsNil(sliceStartIndex)) {
                indexStart = 0;
            } else {
                indexStart = Fox_AsInt(sliceStartIndex);

                if (indexStart < 0) {
                    indexStart = 0;
                }
            }

            switch (Fox_ObjectType(objectValue))
            {
                case OBJ_ARRAY:
                {
                    ObjectArray *pNewArray = gc.New<ObjectArray>();
                    Push(Fox_Object(pNewArray));
                    ObjectArray *pArray = Fox_AsArray(objectValue);

                    if (Fox_IsNil(sliceEndIndex)) {
                        indexEnd = pArray->m_vValues.size();
                    } else {
                        indexEnd = Fox_AsInt(sliceEndIndex);

                        if (indexEnd > pArray->m_vValues.size()) {
                            indexEnd = pArray->m_vValues.size();
                        }
                    }

                    for (int i = indexStart; i < indexEnd; i++) {
                        pNewArray->m_vValues.push_back(pArray->m_vValues[i]);
                    }

                    Pop();
                    returnVal = Fox_Object(pNewArray);

                    break;
                }

                case OBJ_STRING:
                {
                    ObjectString *pString = Fox_AsString(objectValue);

                    if (Fox_IsNil(sliceEndIndex)) {
                        indexEnd = pString->string.size();
                    } else {
                        indexEnd = Fox_AsInt(sliceEndIndex);

                        if (indexEnd > pString->string.size()) {
                            indexEnd = pString->string.size();
                        }
                    }

                    // Ensure the start index is below the end index
                    if (indexStart > indexEnd) {
                        returnVal = Fox_Object(m_oParser.CopyString(""));
                    } else {
                        returnVal = Fox_Object(m_oParser.CopyString(pString->string.substr(indexStart, indexEnd - indexStart)));
                    }
                    break;
                }

                default: {
                    RuntimeError("Can only slice on pArrays and strings.");
                }
            }

            Pop();
            Pop();
            Pop();

            Push(returnVal);
            break;
        }

        case OP_ARRAY:
        {
            Push(Fox_Object(gc.New<ObjectArray>()));
            break;
        }

        case OP_MAP:
        {
            Push(Fox_Object(gc.New<ObjectMap>()));
            break;
        }

        case OP_ADD_LIST:
        {
            int iArgCount = READ_BYTE();
            Value opArrayValue = Peek(iArgCount);

            ObjectArray *pArray = Fox_AsArray(opArrayValue);

            for (int i = iArgCount - 1; i >= 0; i--)
            {
                pArray->m_vValues.push_back(Peek(i));
            }

            m_pCurrentFiber->m_pStackTop -= iArgCount;

            Pop();

            Push(Fox_Object(pArray));
            break;
        }

        case OP_ADD_MAP:
        {
            int iArgCount = READ_BYTE();
            iArgCount *= 2;
            Value oMapValue = Peek(iArgCount);

            ObjectMap *pMap = Fox_AsMap(oMapValue);

            for (int i = iArgCount - 1; i >= 0; i -= 2)
            {
                pMap->m_vValues.Set(Peek(i), Peek(i - 1));
            }

            m_pCurrentFiber->m_pStackTop -= iArgCount;

            Pop();

            Push(Fox_Object(pMap));
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
    ObjectString *b = Fox_AsString(Peek(0));
    ObjectString *a = Fox_AsString(Peek(1));
    Pop();
    Pop();

    ObjectString *result = m_oParser.TakeString(a->string + b->string);
    Push(Fox_Object(result));
}

void VM::AddToRoots()
{
    for (Value *slot = m_pCurrentFiber->m_vStack; slot < m_pCurrentFiber->m_pStackTop; slot++)
        AddValueToRoot(*slot);

    for (int i = 0; i < m_pCurrentFiber->m_iFrameCount; i++)
        AddObjectToRoot(m_pCurrentFiber->m_vFrames[i].closure);

    for (ObjectUpvalue *upvalue = m_pCurrentFiber->m_vOpenUpvalues; upvalue != NULL; upvalue = upvalue->next)
        AddObjectToRoot(upvalue);

    for (auto& handle : m_vHandles)
    {
        AddObjectToRoot(handle);
    }
    
    AddTableToRoot(modules);
    AddTableToRoot(arrayMethods);
    AddCompilerToRoots();
    AddObjectToRoot(initString);
}

void VM::AddTableToRoot(Table &table) {
    for (auto& entry : table.m_vEntries) {
        AddObjectToRoot(entry.m_pKey);
        AddValueToRoot(entry.m_oValue);
    }
}

void VM::AddValueToRoot(Value value) {
    if (!Fox_IsObject(value))
        return;
    AddObjectToRoot(Fox_AsObject(value));
}

void VM::AddObjectToRoot(Object *object) {
    if (object == NULL)
        return;
#ifdef DEBUG_LOG_GC
    printf("%p added to root ", (void *)object);
    PrintValue(Fox_Object(object));
    printf("\n");
#endif
    gc.AddRoot(object);

    BlackenObject(object);
    strings.RemoveWhite();
}

void VM::AddCompilerToRoots()
{
    Compiler *compiler = m_oParser.currentCompiler;
    while (compiler != NULL) {
        AddObjectToRoot((Object *)compiler->function);
        compiler = compiler->enclosing;
    }
}

void VM::AddArrayToRoot(ValueArray *array)
{
    for (int i = 0; i < array->m_vValues.size(); i++)
        AddValueToRoot(array->m_vValues[i]);
}

void VM::BlackenObject(Object *object)
{
#ifdef DEBUG_LOG_GC
    printf("%p blacken ", (void *)object);
    PrintValue(Fox_Object(object));
    printf("\n");
#endif
    switch (object->type) {
    case OBJ_INSTANCE:
    {
        ObjectInstance *instance = (ObjectInstance *)object;
        AddObjectToRoot((Object *)instance->klass);
        AddTableToRoot(instance->fields);
        AddTableToRoot(instance->setters);
        AddTableToRoot(instance->getters);
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
    case OBJ_ARRAY:
    {
        ObjectArray* pArray = (ObjectArray *) object;
        for (auto& oValue : pArray->m_vValues)
            AddValueToRoot(oValue);
        break;
    }

    case OBJ_MAP:
    {
        ObjectMap* pMap = (ObjectMap *) object;
        for (auto& oValue : pMap->m_vValues.m_vEntries)
        {
            AddValueToRoot(oValue.m_oKey);
            AddValueToRoot(oValue.m_oValue);
        }
        break;
    }
    case OBJ_MODULE:
    {
        ObjectModule* pModule = (ObjectModule *) object;
        AddObjectToRoot(pModule->m_strName);
        AddTableToRoot(pModule->m_vVariables);
        break;
    }

    case OBJ_HANDLE:
    {
        Handle* pHandle = (Handle *) object;
        AddValueToRoot(pHandle->value);
        break;
    }
    case OBJ_LIB:
    {
        ObjectLib* pLib = (ObjectLib *) object;
        AddTableToRoot(pLib->methods);
        AddObjectToRoot(pLib->name);
        break;
    }
    case OBJ_NATIVE:
    case OBJ_STRING:
        break;
    }
}

void VM::DefineMethod(ObjectString *name)
{
    Value method = Peek(0);
    ObjectClass *klass = Fox_AsClass(Peek(1));
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

    ObjectBoundMethod *bound = gc.New<ObjectBoundMethod>(Peek(0), Fox_AsClosure(method));
    Pop();
    Push(Fox_Object(bound));
    return true;
}

Value VM::FindVariable(ObjectModule* module, const char* name)
{
    Value oValue;
    if (module->m_vVariables.Get(m_oParser.CopyString(name), oValue))
        return oValue;
    return Fox_Nil;
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
    FOX_ASSERT(!(oVariable == Fox_Nil), "Could not find variable.");
    
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
        pModule->m_vVariables.Set(Fox_AsString(NewString(strName)), oValue);
    }
}

// Looks up the previously loaded module with [name].
//
// Returns `NULL` if no module with that name has been loaded.
ObjectModule* VM::GetModule(Value name)
{
    Value moduleValue;
    if (Fox_IsString(name) && modules.Get(Fox_AsString(name), moduleValue))
        return Fox_AsModule(moduleValue);
    return NULL;
}

ObjectClosure* VM::CompileInModule(Value name, const char* source, bool isExpression, bool printErrors)
{
    // See if the module has already been loaded.
    ObjectModule* module = GetModule(name);
    if (module == NULL)
    {
        module = gc.New<ObjectModule>(Fox_AsString(name));

        // It's possible for the wrenMapSet below to resize the modules map,
        // and trigger a GC while doing so. When this happens it will collect
        // the module we've just created. Once in the map it is safe.
        Push(Fox_Object(module));

        // Store it in the VM's module registry so we don't load the same module
        // multiple times.
        modules.Set(Fox_AsString(name), Fox_Object(module));

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
    Push(Fox_Object(fn));
    ObjectClosure* closure = gc.New<ObjectClosure>(this, fn);
    Pop(); // fn.

    return closure;
}

Value VM::ImportModule(Value name)
{
    // name = resolveModule(vm, name);
    
    // If the module is already loaded, we don't need to do anything.
    Value existing;
    if (modules.Get(Fox_AsString(name), existing))
        return existing;
    
    Push(name);

    const char* source = NULL;
    bool allocatedSource = true;
    
    // If the host didn't provide it, see if it's a built in optional module.
    // if (source == NULL)
    // {
    ObjectString* nameString = Fox_AsString(name);
    nameString->string += ".fox";

    std::ifstream t(nameString->string);
    std::string str((   std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
    source = str.c_str();
    allocatedSource = false;
    // }
    
    if (source == NULL)
    {
        RuntimeError("Could not load module '%s'.", Fox_AsCString(name));
        Pop(); // name.
        return Fox_Nil;
    }
    
    ObjectClosure* moduleClosure = CompileInModule(name, source, false, true);
    
    // Modules loaded by the host are expected to be dynamically allocated with
    // ownership given to the VM, which will free it. The built in optional
    // modules are constant strings which don't need to be freed.
    // if (allocatedSource) DEALLOCATE(vm, (char*)source);
    
    if (moduleClosure == NULL)
    {
        RuntimeError("Could not compile module '%s'.", Fox_AsCString(name));
        Pop(); // name.
        return Fox_Nil;
    }

    Pop(); // name.

    // Return the closure that executes the module.
    return Fox_Object(moduleClosure);
}

Value VM::GetModuleVariable(ObjectModule* module, Value variableName)
{
    ObjectString* variable = Fox_AsString(variableName);
    Value oModule;

    // It's a runtime error if the imported variable does not exist.
    if (module->m_vVariables.Get(Fox_AsString(variableName), oModule))
        return oModule;
    
    RuntimeError("Could not find a variable named '%s' in module '%s'.", Fox_AsCString(variableName), module->m_strName->string.c_str());
    return Fox_Nil;
}

InterpretResult VM::Call(Handle* pMethod)
{
    FOX_ASSERT(pMethod != NULL, "Method cannot be NULL.");
    FOX_ASSERT(Fox_IsClosure(pMethod->value), "Method must be a method handle.");
    FOX_ASSERT(m_pApiStack != NULL, "Must set up arguments for call first.");
    FOX_ASSERT(m_pCurrentFiber != NULL, "Must set up arguments for call first.");
    
    ObjectClosure* closure = Fox_AsClosure(pMethod->value);

    FOX_ASSERT(m_pCurrentFiber->m_pStackTop - m_pApiStack >= closure->function->arity, "Stack must have enough arguments for method.");

    m_pApiStack = NULL;
    CallFunction(closure, closure->function->arity);
    InterpretResult result = run(m_pCurrentFiber);
    m_pApiStack = m_pCurrentFiber->m_vStack;
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
    if (Fox_IsObject(value)) Push(value);
    
    // Make a handle for it.
    Handle* handle = gc.New<Handle>();
    handle->value = value;

    if (Fox_IsObject(value)) Pop();

    // Add it to the front of the linked pArray of handles.
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
    Handle* value = MakeHandle(Fox_Object(fn));
    value->value = Fox_Object(gc.New<ObjectClosure>(this, fn));

    fn->chunk.WriteChunk((uint8_t)OP_CALL, 0);
    fn->chunk.WriteChunk((uint8_t)numParams, 0);
    fn->chunk.WriteChunk((uint8_t)OP_RETURN, 0);
    fn->name = Fox_AsString(NewString(signature));
    return value;
}

void VM::ReleaseHandle(Handle* handle)
{
    FOX_ASSERT(handle != NULL, "Handle cannot be NULL.");

    handle->value = Fox_Nil;
    std::vector<Handle*>::iterator it = std::find(m_vHandles.begin(), m_vHandles.end(), handle);
    m_vHandles.erase(it);
    delete handle;
}

int VM::GetSlotCount()
{
    if (m_pApiStack == NULL) return 0;
    return (int)(m_pCurrentFiber->m_pStackTop - m_pApiStack);
}

void VM::EnsureSlots(int numSlots)
{
    for (int i = 0; i < numSlots; i++)
        Push(Fox_Nil);

    m_pApiStack = m_pCurrentFiber->m_pStackTop - numSlots;
}

// Ensures that [slot] is a valid index into the API's stack of slots.
void VM::ValidateApiSlot(int iSlot)
{
    FOX_ASSERT(iSlot >= 0, "Slot cannot be negative.");
    FOX_ASSERT(iSlot < GetSlotCount(), "Not that many slots.");
}


// Gets the type of the object in [slot].
ValueType VM::GetSlotType(int iSlot)
{
    ValidateApiSlot(iSlot);
    if (Fox_IsBool(m_pApiStack[iSlot])) return VAL_BOOL;
    if (Fox_IsDouble(m_pApiStack[iSlot])) return VAL_NUMBER;
//   if (IS_pArray(m_pApiStack[slot])) return WREN_TYPE_pArray;
//   if (Fox_IsMap(m_pApiStack[slot])) return WREN_TYPE_MAP;
    if (Fox_IsNil(m_pApiStack[iSlot])) return VAL_NIL;
//   if (Fox_IsString(m_pApiStack[slot])) return WREN_TYPE_STRING;
    return VAL_NIL;
}

Value VM::GetSlot(int iSlot)
{
    ValidateApiSlot(iSlot);

    return m_pApiStack[iSlot];
}

bool VM::GetSlotBool(int iSlot)
{
    ValidateApiSlot(iSlot);
    FOX_ASSERT(Fox_IsBool(m_pApiStack[iSlot]), "Slot must hold a bool.");

    return Fox_AsBool(m_pApiStack[iSlot]);
}

double VM::GetSlotDouble(int iSlot)
{
    ValidateApiSlot(iSlot);
    FOX_ASSERT(Fox_IsDouble(m_pApiStack[iSlot]), "Slot must hold a number.");

    return Fox_AsDouble(m_pApiStack[iSlot]);
}

const char* VM::GetSlotString(int iSlot)
{
    ValidateApiSlot(iSlot);
    FOX_ASSERT(Fox_IsString(m_pApiStack[iSlot]), "Slot must hold a string.");

    return Fox_AsCString(m_pApiStack[iSlot]);
}

Handle* VM::GetSlotHandle(int iSlot)
{
    ValidateApiSlot(iSlot);
    return MakeHandle(m_pApiStack[iSlot]);
}

// Stores [value] in [slot] in the foreign call stack.
void VM::SetSlot(int iSlot, Value value)
{
    ValidateApiSlot(iSlot);
    m_pApiStack[iSlot] = value;
}

void VM::SetSlotBool(int iSlot, bool value)
{
    SetSlot(iSlot, Fox_Bool(value));
}

void VM::SetSlotDouble(int iSlot, double value)
{
    SetSlot(iSlot, Fox_Double(value));
}

void VM::SetSlotInteger(int iSlot, int value)
{
    SetSlot(iSlot, Fox_Int(value));
}

void VM::SetSlotNewList(int iSlot)
{
    SetSlot(iSlot, Fox_Object(gc.New<ObjectArray>()));
}

// void wrenSetSlotNewMap(int slot)
// {
//   setSlot(vm, slot, Fox_Object(wrenNewMap(vm)));
// }

void VM::SetSlotNull(int iSlot)
{
    SetSlot(iSlot, Fox_Nil);
}

void VM::SetSlotString(int iSlot, const char* text)
{
    FOX_ASSERT(text != NULL, "String cannot be NULL.\n");
    
    SetSlot(iSlot, NewString(text));
}

void VM::SetSlotHandle(int iSlot, Handle* handle)
{
    FOX_ASSERT(handle != NULL, "Handle cannot be NULL.");

    SetSlot(iSlot, handle->value);
}

int VM::GetListCount(int iSlot)
{
    ValidateApiSlot(iSlot);
    FOX_ASSERT(Fox_IsArray(m_pApiStack[iSlot]), "Slot must hold a pArray.\n");

    return Fox_AsArray(m_pApiStack[iSlot])->m_vValues.size();
}

void VM::GetListElement(int iSlot, int index, int elementSlot)
{
    ValidateApiSlot(iSlot);
    ValidateApiSlot(elementSlot);
    FOX_ASSERT(Fox_IsArray(m_pApiStack[iSlot]), "Slot must hold a pArray.\n");
    
    m_pApiStack[elementSlot] = Fox_AsArray(m_pApiStack[iSlot])->m_vValues[index];
}

void VM::SetListElement(int iSlot, int index, int elementSlot)
{
    ValidateApiSlot(iSlot);
    ValidateApiSlot(elementSlot);
    FOX_ASSERT(Fox_IsArray(m_pApiStack[iSlot]), "Must insert into a pArray.\n");
    
    ObjectArray* pArray = Fox_AsArray(m_pApiStack[iSlot]);
    
    // Negative indices count from the end.
    if (index < 0)
        index = pArray->m_vValues.size() + 1 + index;
    
    FOX_ASSERT(index <= pArray->m_vValues.size(), "Index out of bounds.\n");

    pArray->m_vValues[index] = m_pApiStack[elementSlot];
}