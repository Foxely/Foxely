#include <iostream>
#include <stdexcept>
#include <string.h>
#include <fstream>
#include <streambuf>

#include "foxely.h"
#include "library/library.h"

Value importNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 1);
    // if (!oVM->imports.Set(Fox_AsString(args[0]), Fox_Nil))
    //     return Fox_Nil;
    std::ifstream t(Fox_AsCString(args[0]));
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();

    oVM->Push(oVM->ImportModule(args[0]));
    if (Fox_IsClosure(oVM->Peek(0)))
    {
        oVM->CallFunction(Fox_AsClosure(oVM->Peek(0)), 0);
    }
    else
    {
        // The module has already been loaded. Remember it so we can import
        // variables from it if needed.
        oVM->currentModule->m_vVariables.AddAll(Fox_AsModule(oVM->Pop())->m_vVariables);
        // currentModule = Fox_AsModule(Pop());
    }
    return Fox_Nil;
}

void DefineModuleModule(VM* oVM)
{
    NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("load", importNative),
	};
    oVM->DefineModule("module");
    oVM->DefineLib("module", "module", methods);
}