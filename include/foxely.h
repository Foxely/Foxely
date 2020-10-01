#pragma once

#include <string>
#include "Parser.h"
#include "vm.hpp"

class Value;

static inline Value Fox_StringToValue(const std::string& str)
{
    return OBJ_VAL(VM::GetInstance()->m_oParser.TakeString(str));
}

static const char* Fox_ValueToCString(Value val)
{
    return AS_CSTRING(val);
}

// static inline void DefineClass()
// {
// 	uint8_t nameConstant = VM::GetInstance()->m_oParser.IdentifierConstant(name);
// 	DeclareVariable(parser, name);

// 	parser.EmitBytes(OP_CLASS, nameConstant);
// 	DefineVariable(parser, nameConstant);

// 	ClassCompiler classCompiler(name);
// 	classCompiler.enclosing = parser.currentClass;
// 	parser.currentClass = &classCompiler;

// 	NamedVariable(parser, name, false);

// 	parser.Consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");
// 	while (!parser.PeekTokenIsType(TOKEN_RIGHT_BRACE) && !parser.PeekTokenIsType(TOKEN_EOF)) {
// 		Method(parser);
// 	}
// 	parser.Consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");
// 	parser.EmitByte(OP_POP);

// 	parser.currentClass = parser.currentClass->enclosing;
// }