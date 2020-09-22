
#pragma once

#include <stdint.h>
#include "Lexer.h"
#include "ParserHelper.h"
#include "chunk.hpp"
#include "PointerMath.h"
#include "FreeListAllocator.h"
#include "object.hpp"

// class ObjectFunction;

typedef enum {
    // Single-character tokens.
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

    // One or two character tokens.
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

    // Literals.
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    // Keywords.
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE, TOKEN_IMPORT,

    TOKEN_NEW_LINE,
    TOKEN_WS,
    TOKEN_ERROR,
    TOKEN_EOF,
    TOKEN_MAX
} TokenType;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY,
} Precedence;

// typedef struct {
//     Token name;
//     int depth;
//     bool is_captured;
// } Local;

// struct Upvalue {
//     uint8_t index;
//     bool is_local;
// };

// typedef struct {
//     uint8_t index;
//     bool is_local;
// } UpValue;

// typedef enum {
//     TYPE_FUNCTION,
//     TYPE_INITIALIZER,
//     TYPE_METHOD,
//     TYPE_SCRIPT
// } FunctionType;

class Parser;

typedef void (*parse_fn)(Parser& parser, bool can_assign); 
struct ParseRule {
    parse_fn prefix;
    parse_fn infix;
    Precedence precedence;
};

// struct Compiler {
//     Compiler *enclosing;
//     ObjectFunction *function;
//     FunctionType type;
//     Local locals[UINT8_COUNT];
//     int local_count;
//     Upvalue upvalues[UINT8_COUNT];
//     int scope_depth;
// };

// struct ClassCompiler
// {
//     struct ClassCompiler *enclosing;
//     Token name;
//     bool has_superclass;
// };

class Parser : public ParserHelper
{
public:
	Chunk* compilingChunk;
    bool hadError;
    bool panicMode;
    ParseRule rules[TOKEN_MAX];
    void* m_pProgramMemory;
    FreeListAllocator* m_pAllocator;
    // Compiler *comp;
    // ClassCompiler *comp_class;

    Parser();
    void Advance();
    void MarkInitialized();
    void Consume(int oType, const char* message);
	Chunk* GetCurrentChunk();
	void SetCurrentChunk(Chunk& chunk);


	void EmitByte(uint8_t byte);
	void EmitBytes(uint8_t byte1, uint8_t byte2);
	void EmitConstant(Value value);
	uint8_t MakeConstant(Value value);

	void EndCompiler();


    bool Match(int type);
    ParseRule *GetRule(int type);

    ObjectString* AllocateString(const std::string& str);
    ObjectString* CopyString(const std::string& value);
    ObjectString* TakeString(const std::string& value);
};

bool Compile(Parser& parser, const std::string &strText, Chunk* chunk);
void Expression(Parser& parser);
void ParsePrecedence(Parser& parser, Precedence preced);
void Number(Parser& parser, bool can_assign = false);
void Grouping(Parser& parser, bool can_assign = false);
void Unary(Parser& parser, bool can_assign = false);
void Binary(Parser& parser, bool can_assign = false);
void Literal(Parser& parser, bool can_assign = false);
void RuleAnd(Parser& parser, bool can_assign = false);
void RuleOr(Parser& parser, bool can_assign = false);
void Dot(Parser& parser, bool can_assign = false);
void RuleThis(Parser& parser, bool can_assign = false);
void RuleSuper(Parser& parser, bool can_assign = false);
void Variable(Parser& parser, bool can_assign = false);
void String(Parser& parser, bool can_assign = false);
void CallCompiler(Parser& parser, bool can_assign = false);