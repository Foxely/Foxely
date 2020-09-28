#ifndef fox_vm_h
#define fox_vm_h

#include <vector>
#include <string>
#include <time.h>
#include "chunk.hpp"
#include "value.hpp"
#include "object.hpp"
#include "Table.hpp"


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
	ObjectUpvalue* openUpvalues;
	// std::vector<GCObject*> m_oObjects;

	// static VM vm;

    VM();
	~VM();
    InterpretResult interpret(const char* source);

    void ResetStack();
    void Push(Value value);
    Value Pop();
    Value Peek(int distance);
    void RuntimeError(const char* format, ...);

    void EmitByte(uint8_t byte);
    void Concatenate();
	bool CallValue(Value callee, int argCount);
	bool Call(ObjectClosure* closure, int argCount);
	void DefineNative(const std::string& name, NativeFn function);
	ObjectUpvalue* CaptureUpvalue(Value* local);
	void CloseUpvalues(Value* last);

	void AddToRoots();
	void AddTableToRoot(Table& table);
	void AddValueToRoot(Value value);
	void AddObjectToRoot(Object* object);
	void AddCompilerToRoots();
	void AddArrayToRoot(ValueArray* array);
	void BlackenObject(Object* object);

private:
    InterpretResult run();
};

static Value clockNative(int argCount, Value* args)
{
  return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

#endif
