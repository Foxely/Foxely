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

#define UINT8_COUNT (UINT8_MAX + 1)
#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

struct CallFrame {
	ObjectClosure* closure;
	std::vector<uint8_t>::iterator ip;
	Value* slots;
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
    Parser m_oParser;
	Table strings;
	Table globals;
	// Table imports;
	Table modules;
	Table m_oUserModules;
	ObjectUpvalue* openUpvalues;
	ObjectString* initString;
	GC gc;
	ObjectModule* currentModule;

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
	bool Call(ObjectClosure* closure, int argCount);
	void DefineFunction(const std::string &strModule, const std::string& name, NativeFn function);
	void DefineClass(const std::string &strModule, const std::string& name, NativeMethods& functions);
	void DefineLib(const std::string &name, NativeMethods &functions);
	void DefineBuiltIn(Table& methods, NativeMethods &functions);
	void DefineModule(const std::string& strName);

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
	bool InvokeFromNativeClass(ObjectNativeClass *klass, ObjectString *name, int argCount);

	ObjectModule* GetModule(Value name);
	ObjectClosure* CompileInModule(Value name, const char* source, bool isExpression, bool printErrors);
	Value FindVariable(ObjectModule* module, const char* name);
	void GetVariable(const char* module, const char* name, int slot);
	Value ImportModule(Value name);
	Value GetModuleVariable(ObjectModule* module, Value variableName);

	void BeginModule(std::string strModuleName);

private:
    InterpretResult run();
	static VM m_oInstance;

	InterpretResult result;
	bool isInit;
};

static Value clockNative(VM* oVM, int argCount, Value* args)
{
	return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

#endif
