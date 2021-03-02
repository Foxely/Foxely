#ifndef fox_vm_h
#define fox_vm_h

#include <vector>
#include <string>
#include <time.h>
#include <utility>
#include <map>

#include "chunk.hpp"
#include "value.hpp"
#include "object.hpp"

class Parser;
class Callable;
class GC;
class Table;

// struct CallFrame
// {
// 	ObjectClosure* closure;
// 	std::vector<uint8_t>::iterator ip;
// 	Value* slots;
// };

// A handle to a value, basically just a linked list of extra GC roots.
class Handle : public Object
{
public:
	explicit Handle()
	{
		type = OBJ_HANDLE;
	}
	
  	Value value;
};

typedef enum
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
	INTERPRET_ABORT
} InterpretResult;

using NativeMethods = std::map<std::string, NativeFn>;

class VM
{
public:
	VM(int ac, char** av);
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
    void PrintError(ObjectFiber* pFiber, const char *format, ...);

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

	// Garbage Collector
	void AddToRoots();
	void AddTableToRoot(Table& table);
	void AddValueToRoot(Value value);
	void AddObjectToRoot(Object* object);
	void AddCompilerToRoots();
	void AddArrayToRoot(ValueArray* array);
	void BlackenObject(Object* object);

	// Class
	void DefineOperator(ObjectString *name);
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
    
    InterpretResult run(ObjectFiber* pFiber);

	bool IsLogToken() const;
	bool IsLogGC() const;
	bool IsLogTrace() const;

public:
	InterpretResult result;

    Chunk m_oChunk;
    ObjectFiber* m_pCurrentFiber;
    Value* m_pApiStack;
    Parser m_oParser;
	Table strings;
	Table modules;
	ObjectString* initString;
	ObjectString* stringString;
	GC gc;
	ObjectModule* currentModule;
	std::vector<Handle*> m_vHandles;

    Table arrayMethods;
    Table stringMethods;
    Table mapMethods;
    Table fiberMethods;
    Table builtConvMethods;

	int argc;
	char** argv;

private:
	bool isInit;

	// Debug
	bool m_bLogToken;
	bool m_bLogGC;
	bool m_bLogTrace;
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
		
		m_pVM->ResetStack();
        m_pVM->EnsureSlots(iArity + 1);
        m_pVM->SetSlotHandle(0, m_pVariable);

        std::tuple<Args...> tuple = std::make_tuple(args...);
        passArguments(tuple, std::make_index_sequence<iArity>{});

        auto result = m_pVM->Call(m_pMethod);
		if (result == INTERPRET_OK)
			return m_pVM->GetSlot(0);
		return Fox_Nil;
    }

    bool IsValid()
    {
		return Fox_IsClosure(m_pVariable->value);
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

Value clockNative(VM* oVM, int argCount, Value* args);

#endif
