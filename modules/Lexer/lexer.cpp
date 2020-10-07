#include <iostream>
#include <string.h>
#include "Lexer.h"
#include "foxely.h"
#include "lexer.h"

SCY_PLUGIN(LexerPlugin, "Lexer Module", "0.1.0")


Value DefineNative(int argCount, Value* args)
{
    Fox_Arity(argCount, 2, 3);
	Fox_PanicIfNot(Fox_IsString(args[0]) || Fox_IsNumber(args[0]), "Expected string or int value");
	Fox_PanicIfNot(Fox_IsString(args[1]), "Expected string value");
	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);

	if (lexer)
	{
		if (argCount == 2) {
			if (Fox_IsString(args[0]))
				lexer->Define(Fox_ValueToCString(args[0]), Fox_ValueToCString(args[1]));
			else
				lexer->Define(Fox_ValueToNumber(args[0]), Fox_ValueToCString(args[1]));
		} else {
			Fox_PanicIfNot(Fox_IsBool(args[2]), "Expected bool value");
			if (Fox_IsString(args[0]))
				lexer->Define(Fox_ValueToCString(args[0]), Fox_ValueToCString(args[1]), Fox_ValueToBool(args[2]));
			else
				lexer->Define(Fox_ValueToNumber(args[0]), Fox_ValueToCString(args[1]), Fox_ValueToBool(args[2]));
		}
	}
	return NIL_VAL;
}

Value DefineAreaNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 3);
	Fox_PanicIfNot(Fox_IsString(args[0]) && Fox_IsNumber(args[0]), "Expected string value");
	Fox_PanicIfNot(Fox_IsString(args[1]), "Expected string value");
	Fox_PanicIfNot(Fox_IsString(args[2]), "Expected string value");

	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);

	if (!lexer)
	{
		std::cout << "Lexer is null..\n";
		return NIL_VAL;
	}
	
	lexer->DefineArea(Fox_ValueToCString(args[0]), Fox_ValueToCString(args[1])[0], Fox_ValueToCString(args[2])[0]);
    return NIL_VAL;
}

Value ProcessNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
	Fox_PanicIfNot(Fox_IsString(args[0]), "Expected string value");
	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);
	if (!lexer)
	{
		std::cout << "Lexer is null..\n";
		return BOOL_VAL(false);
	}

    return BOOL_VAL(lexer->Process(Fox_ValueToCString(args[0])));
}

Value DumpNative(int argCount, Value* args)
{
	Fox_FixArity(argCount, 0);
	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);
	if (!lexer)
	{
		std::cout << "Lexer is null..\n";
		return NIL_VAL;
	}

	helper::Dump(*lexer);
    return NIL_VAL;
}

Value lexerInitNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
	Lexer* lexer = (Lexer *) Fox_SetInstanceCStruct(args[-1], new Lexer());
	if (!lexer)
	{
		std::cout << "Lexer is null..\n";
	}
    return NIL_VAL;
}

Value lexerDeinitNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
	Lexer* lexer = (Lexer *) Fox_SetInstanceCStruct(args[-1], new Lexer());
	if (lexer)
	{
		delete lexer;
	}
    return NIL_VAL;
}


LexerPlugin::LexerPlugin()
{
	NativeMethods lexerMethods =
	{
		std::make_pair<std::string, NativeFn>("init", lexerInitNative),
		std::make_pair<std::string, NativeFn>("deinit", lexerDeinitNative),
		std::make_pair<std::string, NativeFn>("Define", DefineNative),
        std::make_pair<std::string, NativeFn>("DefineArea", DefineAreaNative),
		std::make_pair<std::string, NativeFn>("Process", ProcessNative),
		std::make_pair<std::string, NativeFn>("Dump", DumpNative),
	};
	m_oMethods = lexerMethods;

	Fox_DefineClass("Lexer", lexerMethods);
}

LexerPlugin::~LexerPlugin()
{
    // std::cout << "LexerPlugin: Destroy" << std::endl;
}

const char* LexerPlugin::GetClassName() const
{
    return "lexer";
}