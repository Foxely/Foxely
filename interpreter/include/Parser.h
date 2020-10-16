
#pragma once

#include <stdint.h>
#include "Lexer.h"
#include "ParserHelper.h"
#include "chunk.hpp"
#include "object.hpp"

#define UINT8_COUNT (UINT8_MAX + 1)

// class ObjectFunction;
class VM;

typedef enum {
    // Single-character tokens.
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

    // One or two character tokens.
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

	TOKEN_COLON, TOKEN_DOUBLE_COLON,

    // Literals.
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    // Keywords.
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE, TOKEN_IMPORT,
    TOKEN_INTERFACE,

    TOKEN_SINGLE_COMMENT,
    TOKEN_MULTI_COMMENT,
    TOKEN_WS,
    TOKEN_MAX,
    TOKEN_EOF = -1,
    TOKEN_NEW_LINE = 83,
    TOKEN_ERROR = 84,
    TOKEN_SHEBANG,
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

typedef struct {
    Token name;
    int depth;
    bool isCaptured;
} Local;

struct Upvalue {
    uint8_t index;
    bool isLocal;
};

// typedef struct {
//     uint8_t index;
//     bool is_local;
// } UpValue;

typedef enum {
    TYPE_FUNCTION,
    TYPE_INITIALIZER,
    TYPE_METHOD,
    TYPE_SCRIPT
} FunctionType;

class Parser;
class Compiler;

typedef void (*parse_fn)(Parser& parser, bool can_assign);

struct ParseRule
{
    parse_fn prefix;
    parse_fn infix;
    Precedence precedence;
};

struct ClassCompiler
{
	explicit ClassCompiler(Token& n) : name(n) {}
   	ClassCompiler *enclosing;
    Token& name;
    bool hasSuperclass;
};

class Parser : public ParserHelper
{
public:
	Chunk* compilingChunk;
    ParseRule rules[TOKEN_MAX];
	VM* m_pVm;
    Compiler *currentCompiler;
	ClassCompiler* currentClass;

    Parser();
	bool IsEnd();
    void Advance();
    void MarkInitialized();
    void Consume(int oType, const char* message);
	Chunk* GetCurrentChunk();
	void SetCurrentChunk(Chunk& chunk);

	ObjectString* CopyString(const std::string& value);
	ObjectString* TakeString(const std::string& value);
	ObjectString* AllocateString(const std::string& str, uint32_t hash);
	uint32_t hashString(const std::string& str);


	void EmitByte(uint8_t byte);
	void EmitBytes(uint8_t byte1, uint8_t byte2);
	void EmitConstant(Value value);
	void EmitReturn();
	int EmitJump(uint8_t instruction);
	void PatchJump(int offset);
	void EmitLoop(int loopStart);
	uint8_t MakeConstant(Value value);

	ObjectFunction* EndCompiler();


    bool Match(int type);
    ParseRule *GetRule(int type);

	uint8_t IdentifierConstant(const Token& name);

	void BeginScope();
	void EndScope();
};

struct Compiler
{
	explicit Compiler (Parser& parser, FunctionType eType, const std::string& name)
	{
		enclosing = parser.currentCompiler;
		localCount = 0;
		scopeDepth = 0;
		type = eType;
		function = new ObjectFunction();
		parser.currentCompiler = this;

		if (eType != TYPE_SCRIPT) {
			parser.currentCompiler->function->name = parser.CopyString(name);
		}

		Local* local = &locals[localCount++];
		local->depth = 0;
        local->isCaptured = false;

		if (type != TYPE_FUNCTION) {
			local->name = Token("this", (std::size_t) 4);
		} else {
			local->name = Token("", (std::size_t) 0);
		}
	}

    Compiler *enclosing;
    ObjectFunction *function;
    FunctionType type;
    Local locals[UINT8_COUNT];
    int localCount;
    Upvalue upvalues[UINT8_COUNT];
    int scopeDepth;
};

ObjectFunction* Compile(Parser& parser, const std::string &strText, Chunk* chunk);
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
void Subscript(Parser& parser, bool canAssign);
void List(Parser& parser, bool canAssign);

void Block(Parser& parser);
void Declaration(Parser& parser);
void Synchronize(Parser& parser);
void VarDeclaration(Parser& parser, Token name);
// void VarDeclaration(Parser& parser);
void NamedVariable(Parser& parser, Token name, bool can_assign);
void AddLocal(Parser& parser, Token name);
bool IdentifiersEqual(Token& a, Token& b);
int ResolveLocal(Parser& parser, Compiler *comp, Token& name);

void Function(Parser& parser, FunctionType type, const Token& name);
void FuncDeclaration(Parser& parser, Token name);

void InterfaceDeclaration(Parser& parser, Token name);
void Interface(Parser& parser, FunctionType type, const Token& name);

int ResolveUpvalue(Parser& parser, Compiler *comp, Token& name);
void ClassDeclaration(Parser& parser, Token& name);
uint8_t ArgumentList(Parser& parser);
void DeclareVariable(Parser& parser, Token name);
void DefineVariable(Parser& parser, uint8_t global);
