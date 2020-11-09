#ifndef fox_vm_h
#define fox_vm_h

#pragma once

#include <vector>
#include <string>
#include <time.h>
#include <utility>
#include <map>

#include "chunk.hpp"
#include "value.hpp"
#include "Table.hpp"
#include "object.hpp"
#include "gc.hpp"

class Parser;
class Callable;

#define UINT8_COUNT (UINT8_MAX + 1)
#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

struct CallFrame
{
	ObjectClosure* closure;
	std::vector<uint8_t>::iterator ip;
	Value* slots;
};

// A handle to a value, basically just a linked list of extra GC roots.
class Handle : public Object
{
public:
  	Value value;
};

typedef enum
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} InterpretResult;

using NativeMethods = std::map<std::string, NativeFn>;

class VM
{
public:
	CallFrame frames[FRAMES_MAX];
  	int frameCount;

    Chunk m_oChunk;
    std::vector<uint8_t>::iterator ip;
    Value stack[STACK_MAX];
    Value* stackTop;
    Value* m_pApiStack;
    Parser m_oParser;
	Table strings;
	// Table globals;
	// Table imports;
	Table modules;
	// Table m_oUserModules;
	ObjectUpvalue* openUpvalues;
	ObjectString* initString;
	GC gc;
	ObjectModule* currentModule;
	std::vector<Handle*> m_vHandles;

    Table arrayMethods;

	int argc;
	char** argv;

    VM();
	~VM();

	void Load();
	void LoadStandard(const std::string& name);

	Value NewString(const char* string);

    InterpretResult Interpret(const char* module, const char* source);
	ObjectClosure* CompileSource(const char* module, const char* source, bool isExpression, bool printErrors);

    void ResetStack();
    void Push(Value value);
    Value Pop();
    Value Peek(int distance);
	Value PeekStart(int distance);
    void RuntimeError(const char* format, ...);

    void EmitByte(uint8_t byte);
    void Concatenate();
	bool CallValue(Value callee, int argCount);
	bool CallFunction(ObjectClosure* closure, int argCount);
	
	void DefineFunction(const std::string &strModule, const std::string& name, NativeFn function);
	void DefineClass(const std::string &strModule, const std::string& name, NativeMethods& functions);
	void DefineLib(const std::string &strModule, const std::string &name, NativeMethods &functions);
	void DefineBuiltIn(Table& methods, NativeMethods &functions);
	void DefineModule(const std::string& strName);
	void DefineVariable(const char* module, const char* name, Value oValue);
	
	InterpretResult Call(Handle* pMethod);
	// template <typename... Args>
	// InterpretResult Call(Handle* pMethod, Args&&... args);
	void ReleaseHandle(Handle* handle);
	Handle* MakeHandle(Value value);
    Handle* MakeCallHandle(const char* signature);

	ObjectUpvalue* CaptureUpvalue(Value* local);
	void CloseUpvalues(Value* last);

	void AddToRoots();
	void AddTableToRoot(Table& table);
	void AddValueToRoot(Value value);
	void AddObjectToRoot(Object* object);
	void AddCompilerToRoots();
	void AddArrayToRoot(ValueArray* array);
	void BlackenObject(Object* object);
	void DefineMethod(ObjectString* name);
	bool BindMethod(ObjectClass* klass, ObjectString* name);
	bool Invoke(ObjectString* name, int argCount);
	bool InvokeFromClass(ObjectClass* klass, ObjectString* name, int argCount);

	ObjectModule* GetModule(Value name);
	ObjectClosure* CompileInModule(Value name, const char* source, bool isExpression, bool printErrors);
	Value FindVariable(ObjectModule* module, const char* name);
	void GetVariable(const char* module, const char* name, int slot);
	Value ImportModule(Value name);
	Value GetModuleVariable(ObjectModule* module, Value variableName);

	Callable Function(const std::string& strModuleName, const std::string& strSignature);

	int GetSlotCount();
	void EnsureSlots(int numSlots);
	void ValidateApiSlot(int slot);

    ValueType GetSlotType(int slot);
	Value GetSlot(int slot);
    bool GetSlotBool(int slot);
    double GetSlotDouble(int slot);
    const char* GetSlotString(int slot);
    Handle* GetSlotHandle(int slot);

	void SetSlot(int slot, Value value);
	void SetSlotBool(int slot, bool value);
	void SetSlotInteger(int slot, int value);
	void SetSlotDouble(int slot, double value);
	void SetSlotNewList(int slot);
	void SetSlotNull(int slot);
	void SetSlotString(int slot, const char* text);
    void SetSlotHandle(int slot, Handle* handle);
	int GetListCount(int slot);
	void GetListElement(int listSlot, int index, int elementSlot);
	void SetListElement(int listSlot, int index, int elementSlot);

private:
    InterpretResult run();
	static VM m_oInstance;

	InterpretResult result;
	bool isInit;
};

struct ExpandType
{
    template<typename... T>
    ExpandType(T&&...)
    {
    }
};

class Callable
{
public:
    Handle* m_pVariable;
    Handle* m_pMethod;
    VM* m_pVM;

	template<typename... Args>
    Value Call(Args... args)
    {
		constexpr const int iArity = sizeof...(Args);
		
        m_pVM->EnsureSlots(iArity + 1);
        m_pVM->SetSlotHandle(0, m_pVariable);

        std::tuple<Args...> tuple = std::make_tuple(args...);
        passArguments(tuple, std::make_index_sequence<iArity>{});

        auto result = m_pVM->Call(m_pMethod);
		if (result == INTERPRET_OK)
			return m_pVM->GetSlot(0);
		return NIL_VAL;
    }

    bool IsValid()
    {
		return IS_CLOSURE(m_pVariable->value);
    }

    void Release()
    {
		m_pVM->ReleaseHandle(m_pVariable);
		m_pVM->ReleaseHandle(m_pMethod);
    }

    template <typename T, std::size_t index> void read(T value)
    {
        if (typeid(T) == typeid(int) || typeid(T) == typeid(unsigned int))
            m_pVM->SetSlotInteger(index + 1, value);
        else if (typeid(T) == typeid(float) || typeid(T) == typeid(double))
            m_pVM->SetSlotDouble(index + 1, value);
        else if (typeid(T) == typeid(bool))
            m_pVM->SetSlotBool(index + 1, value);
    }

    template <typename... Args, std::size_t... index>
    void passArguments(const std::tuple<Args...>& tuple, std::index_sequence<index...>) 
    {
        ExpandType {0, (read<Args, index>(std::get<index>(tuple)), 0)...};
    }
};

// template <typename... Args>
// inline InterpretResult VM::Call(Handle* pMethod, Args&&... args)
// {
//     constexpr auto count = sizeof...(Args);
// 	for (int i = 0; i < count; i++)
// 	{
// 		if (typeid(decltype(args[i])).name() == "int")
// 		{
// 			std::cout << "int" << std::endl;
// 		}
// 	}
//     std::cout << "Size: " << count << std::endl;
//     return Call(pMethod);
// }

inline Value clockNative(VM* oVM, int argCount, Value* args)
{
	return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

#endif
