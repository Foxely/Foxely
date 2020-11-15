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
	return Fox_Nil;
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
		return Fox_Nil;
	}
	
	lexer->DefineArea(Fox_ValueToCString(args[0]), Fox_ValueToCString(args[1])[0], Fox_ValueToCString(args[2])[0]);
    return Fox_Nil;
}

Value ProcessNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
	Fox_PanicIfNot(Fox_IsString(args[0]), "Expected string value");
	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);
	if (!lexer)
	{
		std::cout << "Lexer is null..\n";
		return Fox_Bool(false);
	}

    return Fox_Bool(lexer->Process(Fox_ValueToCString(args[0])));
}

Value DumpNative(int argCount, Value* args)
{
	Fox_FixArity(argCount, 0);
	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);
	if (!lexer)
	{
		std::cout << "Lexer is null..\n";
		return Fox_Nil;
	}

	helper::Dump(*lexer);
    return Fox_Nil;
}

Value TokenMatchNative(int argCount, Value* args)
{
	Fox_FixArity(argCount, 2);
	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);
	Token* token = (Token *) Fox_GetInstanceCStruct(args[0]);
	Fox_PanicIfNot(Fox_IsString(args[1]), "Expected string value");

	return Fox_Bool(lexer->TokenMatch(*token, Fox_ValueToCString(args[1])));
}

Value BeginNative(int argCount, Value* args)
{
	Fox_FixArity(argCount, 0);
	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);

	lexer->Begin();
	return Fox_Nil;
}

Value IsEndNative(int argCount, Value* args)
{
	Fox_FixArity(argCount, 0);
	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);

	return Fox_Bool(lexer->oTokenIterator == lexer->oTokenList.end());
}

Value NextTokenNative(int argCount, Value* args)
{
	Fox_FixArity(argCount, 0);
	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);
	Token* t = new Token(lexer->NextToken());

	return Fox_DefineInstanceOfCStruct("Token", t);
}

Value lexerInitNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
	Lexer* lexer = (Lexer *) Fox_SetInstanceCStruct(args[-1], new Lexer());
	if (!lexer)
	{
		std::cout << "Lexer is null..\n";
	}
    return Fox_Nil;
}

Value lexerDeinitNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
	Lexer* lexer = (Lexer *) Fox_GetInstanceCStruct(args[-1]);
	
	if (lexer)
	{
		delete lexer;
	}
    return Fox_Nil;
}




Value tokenInitNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
	Fox_SetInstanceCStruct(args[-1], nullptr);

    return Fox_Nil;
}

Value tokenDeinitNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
	Token* token = (Token *) Fox_GetInstanceCStruct(args[-1]);
	if (token)
	{
		delete token;
	}
    return Fox_Nil;
}

Value tokenGetTypeNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
	Token* token = (Token *) Fox_GetInstanceCStruct(args[-1]);

    return Fox_Double(token->m_oType.m_id);
}

Value tokenGetLineNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
	Token* token = (Token *) Fox_GetInstanceCStruct(args[-1]);

    return Fox_Double(token->m_iLinesTraversed);
}

Value tokenGetTextNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
	Token* token = (Token *) Fox_GetInstanceCStruct(args[-1]);

    return Fox_StringToValue(token->GetText().c_str());
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
		std::make_pair<std::string, NativeFn>("TokenMatch", TokenMatchNative),
		std::make_pair<std::string, NativeFn>("Begin", BeginNative),
		std::make_pair<std::string, NativeFn>("IsEnd", IsEndNative),
		std::make_pair<std::string, NativeFn>("NextToken", NextTokenNative),
	};

	NativeMethods tokenMethods =
	{
		std::make_pair<std::string, NativeFn>("init", tokenInitNative),
		std::make_pair<std::string, NativeFn>("deinit", tokenDeinitNative),
		std::make_pair<std::string, NativeFn>("getType", tokenGetTypeNative),
		std::make_pair<std::string, NativeFn>("getLine", tokenGetLineNative),
		std::make_pair<std::string, NativeFn>("getText", tokenGetTextNative),
	};

	Fox_DefineClass("Lexer", lexerMethods);
	Fox_DefineClass("Token", tokenMethods);
}

LexerPlugin::~LexerPlugin()
{
    // std::cout << "LexerPlugin: Destroy" << std::endl;
}

const char* LexerPlugin::GetClassName() const
{
    return "lexer";
}