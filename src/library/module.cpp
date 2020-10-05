#include "library/module.h"
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include "foxely.h"

Value importNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    if (!VM::GetInstance()->imports.Set(AS_STRING(args[0]), NIL_VAL))
        return NIL_VAL;
    std::ifstream t(Fox_ValueToCString(args[0]));
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();

    Chunk oChunk;
	ObjectFunction *function = Compile(VM::GetInstance()->m_oParser, str.c_str(), &oChunk);
    if (function == NULL)
        return NIL_VAL;
    VM::GetInstance()->Push(OBJ_VAL(function));
    ObjectClosure *closure = new ObjectClosure(function);
    VM::GetInstance()->Pop();
    VM::GetInstance()->Call(closure, 0);
    CallFrame *frame = &VM::GetInstance()->frames[VM::GetInstance()->frameCount - 1];
    frame = &VM::GetInstance()->frames[VM::GetInstance()->frameCount - 1];
    return NIL_VAL;
}


ModulePlugin::ModulePlugin()
{
    // std::cout << "ModulePlugin: Create" << std::endl;
}

ModulePlugin::~ModulePlugin()
{
    // std::cout << "ModulePlugin: Destroy" << std::endl;
}

const char* ModulePlugin::GetClassName() const
{
    return "module";
}

NativeMethods ModulePlugin::GetMethods()
{
	NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("load", importNative),
	};
    
	return methods;
}