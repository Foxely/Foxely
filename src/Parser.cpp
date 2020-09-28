
#include <cstring>
#include "Parser.h"
#include "compiler.h"
#include "object.hpp"
#include "vm.hpp"
#include "gc.hpp"
#include "gc_allocator.hpp"
#include "statement.hpp"


Chunk* Parser::GetCurrentChunk()
{
  	// return compilingChunk;
	return &currentCompiler->function->chunk;
}

void Parser::SetCurrentChunk(Chunk& chunk)
{
  	compilingChunk = &chunk;
}

Parser::Parser()
{
	currentCompiler = NULL;

    oLexer.Define(TOKEN_NEW_LINE,"\n", true);
    oLexer.Define(TOKEN_WS,"[ \t\r\b]+", true);
    oLexer.Define(TOKEN_NUMBER,"[0-9]+");
    oLexer.DefineArea(TOKEN_STRING,'"', '"');
	oLexer.Define(TOKEN_PLUS, "\\+");
	oLexer.Define(TOKEN_MINUS, "-");
    oLexer.Define(TOKEN_DOUBLE_COLON,"::");
    oLexer.Define(TOKEN_COLON,":");
    oLexer.Define(TOKEN_LEFT_PAREN,"\\(");
    oLexer.Define(TOKEN_RIGHT_PAREN,"\\)");
    oLexer.Define(TOKEN_LEFT_BRACE,"\\{");
    oLexer.Define(TOKEN_RIGHT_BRACE,"\\}");
	oLexer.Define(TOKEN_DOT, "\\.");
    oLexer.Define(TOKEN_SEMICOLON,";");
    oLexer.Define(TOKEN_SLASH,"/");
    oLexer.Define(TOKEN_COMMA,",");
    oLexer.Define(TOKEN_STAR,"\\*");
    oLexer.Define(TOKEN_GREATER,">");
    oLexer.Define(TOKEN_GREATER_EQUAL,">=");
    oLexer.Define(TOKEN_LESS, "<");
    oLexer.Define(TOKEN_LESS_EQUAL, "<=");
    oLexer.Define(TOKEN_NIL, "nil");
    oLexer.Define(TOKEN_OR, "\\|\\|");
    oLexer.Define(TOKEN_AND, "&&");
    oLexer.Define(TOKEN_PRINT, "print");
    oLexer.Define(TOKEN_SUPER, "super");
    oLexer.Define(TOKEN_THIS, "this");
    oLexer.Define(TOKEN_TRUE, "true");
    oLexer.Define(TOKEN_FALSE, "false");
    oLexer.Define(TOKEN_FOR, "for");
    oLexer.Define(TOKEN_FUN, "fun");
    oLexer.Define(TOKEN_IF, "if");
    oLexer.Define(TOKEN_IMPORT, "import");
    oLexer.Define(TOKEN_RETURN, "return");
    oLexer.Define(TOKEN_VAR, "var");
    oLexer.Define(TOKEN_WHILE, "while");
    oLexer.Define(TOKEN_BANG, "!");
    oLexer.Define(TOKEN_BANG_EQUAL, "!=");
    oLexer.Define(TOKEN_CLASS, "class");
    oLexer.Define(TOKEN_ELSE, "else");
    oLexer.Define(TOKEN_EQUAL, "=");
    oLexer.Define(TOKEN_EQUAL_EQUAL, "==");
    oLexer.Define(TOKEN_IDENTIFIER,"[A-Za-z]+[0-9]*");
    // oLexer.Define("Open Square Bracket","\\[");
    // oLexer.Define("Close Square Bracket","\\]");
    // oLexer.Define("Arrow","->");
    oLexer.Define(TOKEN_SINGLE_COMMENT,"//.*\n", true);
    oLexer.Define(TOKEN_MULTI_COMMENT,"/\\*.*\\*/", true);
    oLexer.Define(TOKEN_EOF,"[\0]");


    rules[TOKEN_LEFT_PAREN] = { Grouping, CallCompiler, PREC_CALL };
    rules[TOKEN_RIGHT_PAREN] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_LEFT_BRACE] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_RIGHT_BRACE] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_COMMA] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_DOT] = { NULL, Dot, PREC_CALL };
    rules[TOKEN_MINUS] = { Unary, Binary, PREC_TERM };
    rules[TOKEN_PLUS] = { NULL, Binary, PREC_TERM };
    rules[TOKEN_SEMICOLON] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_SLASH] = { NULL, Binary, PREC_FACTOR };
    rules[TOKEN_STAR] = { NULL, Binary, PREC_FACTOR };
    rules[TOKEN_BANG] = { Unary, NULL, PREC_NONE };
    rules[TOKEN_BANG_EQUAL] = { NULL, Binary, PREC_EQUALITY };
    rules[TOKEN_EQUAL] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_EQUAL_EQUAL] = { NULL, Binary, PREC_EQUALITY };
    rules[TOKEN_GREATER] = { NULL, Binary, PREC_COMPARISON };
    rules[TOKEN_GREATER_EQUAL] = { NULL, Binary, PREC_COMPARISON };
    rules[TOKEN_LESS] = { NULL, Binary, PREC_COMPARISON };
    rules[TOKEN_LESS_EQUAL] = { NULL, Binary, PREC_COMPARISON };
    rules[TOKEN_IDENTIFIER] = { Variable, NULL, PREC_NONE };
    rules[TOKEN_STRING] = { String, NULL, PREC_NONE };
    rules[TOKEN_NUMBER] = { Number, NULL, PREC_NONE };
    rules[TOKEN_AND] = { NULL, RuleAnd, PREC_NONE };
    rules[TOKEN_CLASS] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_ELSE] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_FALSE] = { Literal, NULL, PREC_NONE };
    rules[TOKEN_FOR] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_FUN] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_IF] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_NIL] = { Literal, NULL, PREC_NONE };
    rules[TOKEN_OR] = { NULL, RuleOr, PREC_NONE };
    rules[TOKEN_PRINT] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_RETURN] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_SUPER] = { RuleSuper, NULL, PREC_NONE };
    rules[TOKEN_THIS] = { RuleThis, NULL, PREC_NONE };
    rules[TOKEN_TRUE] = { Literal, NULL, PREC_NONE };
    rules[TOKEN_VAR] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_WHILE] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_ERROR] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_EOF] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_COLON] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_DOUBLE_COLON] = { NULL, NULL, PREC_NONE };
}

bool Parser::IsEnd()
{
	return GetLexer().oTokenIterator == GetLexer().oTokenList.end();
}

void Parser::Advance()
{
    NextToken();
}

void Parser::MarkInitialized()
{
    if (currentCompiler->scopeDepth == 0)
        return;
    currentCompiler->locals[currentCompiler->localCount - 1].depth = currentCompiler->scopeDepth;
}

ObjectFunction* Compile(Parser& parser, const std::string& strText, Chunk* chunk)
{
    if (!parser.Init(strText))
        return NULL;

	Compiler compiler(parser, TYPE_SCRIPT, "");
	parser.compilingChunk = chunk;
    parser.hadError = false;
    parser.panicMode = false;
#if DEBUG_TOKEN
	helper::Dump(parser.GetLexer());
 #endif

	while (!parser.IsEnd()) {
		Declaration(parser);
	}
	ObjectFunction *func = parser.EndCompiler();
    // parser.Advance();
    return parser.hadError ? NULL : func;
}

void Expression(Parser& parser)
{
    ParsePrecedence(parser, PREC_ASSIGNMENT);
}

void Parser::Consume(int oType, const char* message)
{
	if (!IsToken(oType)) {
		ErrorAtCurrent(message);
	}
}

void Parser::EmitByte(uint8_t byte)
{
    GetCurrentChunk()->WriteChunk(byte, PreviousToken().m_iLinesTraversed);
}

void Parser::EmitBytes(uint8_t byte1, uint8_t byte2)
{
	EmitByte(byte1);
	EmitByte(byte2);
}

void Parser::EmitConstant(Value value)
{
  	EmitBytes(OP_CONST, MakeConstant(value));
}

void Parser::EmitReturn()
{
    if (currentCompiler->type == TYPE_INITIALIZER)
        EmitBytes(OP_GET_LOCAL, 0);
    else
        EmitByte(OP_NIL);
    EmitByte(OP_RETURN);
}

int Parser::EmitJump(uint8_t instruction)
{
	EmitByte(instruction);
	EmitByte(0xff);
	EmitByte(0xff);
	// return compilingChunk->m_iCount - 2;
	return GetCurrentChunk()->m_vCode.size() - 2;
}

void Parser::PatchJump(int offset)
{
	// -2 to adjust for the bytecode for the jump offset itself.
	// int jump = compilingChunk->m_iCount - offset - 2;
	int jump = GetCurrentChunk()->m_vCode.size() - offset - 2;

	if (jump > UINT16_MAX) {
		Error("Too much code to jump over.");
	}

	GetCurrentChunk()->m_vCode[offset] = (jump >> 8) & 0xff;
	GetCurrentChunk()->m_vCode[offset + 1] = jump & 0xff;
}

void Parser::EmitLoop(int loopStart)
{
	EmitByte(OP_LOOP);

	int offset = GetCurrentChunk()->m_vCode.size() - loopStart + 2;

	if (offset > UINT16_MAX)
		Error("Loop body too large.");

	EmitByte((offset >> 8) & 0xff);
	EmitByte(offset & 0xff);
}

uint8_t Parser::MakeConstant(Value value)
{

	int constant = GetCurrentChunk()->AddConstant(value);
	if (constant > UINT8_MAX) {
		Error("Too many constants in one chunk.");
		return 0;
	}
  	return (uint8_t) constant;
}

ObjectFunction* Parser::EndCompiler()
{
	EmitReturn();
    ObjectFunction* func = currentCompiler->function;
    #ifdef DEBUG_PRINT_CODE
    if (!hadError) {
        disassemble_chunk(GetCurrentChunk(), func->name != NULL ? func->name->string : "<script>");
    }
    #endif
    currentCompiler = currentCompiler->enclosing;
    return (func);
}


bool Parser::Match(int type)
{
    if (CurrentToken().m_oType.m_id != type)
        return false;
    Advance();
    return true;
}

ParseRule *Parser::GetRule(int type)
{
    return &rules[type];
}

void ParsePrecedence(Parser& parser, Precedence preced)
{
    parser.Advance();
    ParseRule* pRule = parser.GetRule(parser.PreviousToken().m_oType.m_id);
    parse_fn prefix_rule = pRule ? pRule->prefix : nullptr;
    if (prefix_rule == NULL) {
        parser.Error("Expect expression.");
        return;
    }
    bool can_assign = preced <= PREC_ASSIGNMENT;
    prefix_rule(parser, can_assign);
    while (preced <= parser.GetRule(parser.CurrentToken().m_oType.m_id)->precedence) {
        parser.Advance();
        parse_fn infix_rule = parser.GetRule(parser.PreviousToken().m_oType.m_id)->infix;
        infix_rule(parser, can_assign);
    }

    if (can_assign && parser.Match(TOKEN_EQUAL)) {
        parser.Error("Invalid assignment target.");
    }
}


void Number(Parser& parser, bool can_assign)
{
	double value = strtod(parser.PreviousToken().GetText().c_str(), NULL);
	parser.EmitConstant(NUMBER_VAL(value));
}

void Grouping(Parser& parser, bool can_assign)
{
  	Expression(parser);
  	parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Unary(Parser& parser, bool can_assign)
{
    int operator_type = parser.PreviousToken().m_oType.m_id;
    Expression(parser);
    switch (operator_type) {
        case TOKEN_BANG:
            parser.EmitByte(OP_NOT);
            break;
        case TOKEN_MINUS:
            parser.EmitByte(OP_NEGATE);
            break;
        default:
        return;
    }
}


void Binary(Parser& parser, bool can_assign)
{
    int operatorType = parser.PreviousToken().m_oType.m_id;
    ParseRule *rule = parser.GetRule(operatorType);
    ParsePrecedence(parser, (Precedence)(rule->precedence + 1));

    switch (operatorType) {
        case TOKEN_BANG_EQUAL:
            parser.EmitBytes(OP_EQUAL, OP_NOT);
            break;
        case TOKEN_EQUAL_EQUAL:
            parser.EmitByte(OP_EQUAL);
            break;
        case TOKEN_GREATER:
            parser.EmitByte(OP_GREATER);
            break;
        case TOKEN_GREATER_EQUAL:
            parser.EmitBytes(OP_LESS, OP_NOT);
            break;
        case TOKEN_LESS:
            parser.EmitByte(OP_LESS);
            break;
        case TOKEN_LESS_EQUAL:
            parser.EmitBytes(OP_GREATER, OP_NOT);
            break;
        case TOKEN_PLUS:
            parser.EmitByte(OP_ADD);
            break;
        case TOKEN_MINUS:
            parser.EmitByte(OP_SUB);
            break;
        case TOKEN_STAR:
            parser.EmitByte(OP_MUL);
        break;
        case TOKEN_SLASH:
            parser.EmitByte(OP_DIV);
        break;
        default:
            return;
    }
}

void Literal(Parser& parser, bool can_assign)
{
    int operatorType = parser.PreviousToken().m_oType.m_id;
    switch (operatorType) {
        case TOKEN_FALSE:
            parser.EmitByte(OP_FALSE);
            break;
        case TOKEN_NIL:
            parser.EmitByte(OP_NIL);
            break;
        case TOKEN_TRUE:
            parser.EmitByte(OP_TRUE);
            break;
        default:
        return;
    }
}

void RuleAnd(Parser& parser, bool can_assign)
{
    int end_jump = parser.EmitJump(OP_JUMP_IF_FALSE);

    parser.EmitByte(OP_POP);
    ParsePrecedence(parser, PREC_AND);
    parser.PatchJump(end_jump);
}

void RuleOr(Parser& parser, bool can_assign)
{
    int else_jump = parser.EmitJump(OP_JUMP_IF_FALSE);
    int end_jump = parser.EmitJump(OP_JUMP);

    parser.PatchJump(else_jump);
    parser.EmitByte(OP_POP);

    ParsePrecedence(parser, PREC_OR);
    parser.PatchJump(end_jump);
}

void Dot(Parser& parser, bool can_assign)
{
    // Consume(TOKEN_IDENTIFIER, "Expect property name after '.'.");
    // uint8_t name = IdentifierConstant(CurrentToken());

    // if (can_assign && Match(TOKEN_EQUAL)) {
    //     Expression();
    //     EmitBytes(OP_SET_PROPERTY, name);
    // } else if (Match(TOKEN_LEFT_PAREN)) {
    //     uint8_t arg_count = ArgumentList();
    //     EmitBytes(OP_INVOKE, name);
    //     EmitByte(arg_count);
    // } else {
    //     EmitBytes(OP_GET_PROPERTY, name);
    // }
}

void RuleThis(Parser& parser, bool can_assign)
{
    // if (parser->comp_class == NULL) {
    //     Error("Cannot use 'this' outside of a class.");
    //     return;
    // }
    // Variable(false);
}

void RuleSuper(Parser& parser, bool can_assign)
{
    // if (parser->comp_class == NULL) {
    //     Error("Cannot use 'super' outside of a class.");
    // } else if (!parser->comp_class->has_superclass) {
    //     Error("Cannot use 'super' in a class with no superclass.");
    // }
    // Consume(TOKEN_DOT, "Expect '.' after 'super'.");
    // Consume(TOKEN_IDENTIFIER, "Expect superclass method name.");
    // uint8_t name = IdentifierConstant(&parser->previous);
    // NamedVariable(SyntheticToken("this"), false);
    // if (Match(TOKEN_LEFT_PAREN)) {
    //     uint8_t arg_count = ArgumentList();
    //     NamedVariable(SyntheticToken("super"), false);
    //     EmitBytes(OP_SUPER_INVOKE, name);
    //     EmitByte(arg_count);
    // } else {
    //     NamedVariable(SyntheticToken("super"), false);
    //     EmitBytes(OP_GET_SUPER, name);
    // }
}

void Variable(Parser& parser, bool can_assign)
{
	Token name = parser.PreviousToken();
	if (parser.IsToken(TOKEN_COLON) && parser.IsToken(TOKEN_VAR))
	{
		VarDeclaration(parser, name);
	}
	else if (parser.IsToken(TOKEN_DOUBLE_COLON) && parser.IsToken(TOKEN_FUN))
	{
		FuncDeclaration(parser, name);
	}
	else
    	NamedVariable(parser, name, can_assign);
}

void String(Parser& parser, bool can_assign)
{
    parser.EmitConstant(OBJ_VAL(parser.CopyString(parser.PreviousToken().GetText())));
}

uint8_t ArgumentList(Parser& parser)
{
	uint8_t argCount = 0;
	if (!parser.IsToken(TOKEN_RIGHT_PAREN, false)) {
		do
		{
			Expression(parser);
			if (argCount == 255)
				parser.Error("Cannot have more than 255 arguments.");
			argCount++;
		} while (parser.Match(TOKEN_COMMA));
	}
	parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
	return argCount;
}

void CallCompiler(Parser& parser, bool can_assign)
{
    uint8_t arg_count = ArgumentList(parser);
    parser.EmitBytes(OP_CALL, arg_count);
}

void Declaration(Parser& parser)
{
	// Token name;
	// if (parser.IsTokenThenAssign(TOKEN_IDENTIFIER, name))
	// {
		// if (parser.Match(TOKEN_CLASS)) {
        // 	// ClassDeclaration(parser);
		// } else if (parser.Match(TOKEN_FUN)) {
		// 	// FunctionDeclaration(parser);
		// } else {
		// 	if (parser.IsToken(TOKEN_VAR)) {
		// 		// VarDeclaration(parser);
		// 	} else
				Statement(parser);
		// }
	// }
    if (parser.panicMode)
        Synchronize(parser);
}

// bool Check(int type)
// {
// 	return CurrentToken().m_oType.m_id == type;
// }

void Block(Parser& parser)
{
	while (!parser.IsToken(TOKEN_RIGHT_BRACE, false) && !parser.IsToken(TOKEN_EOF, false)) {
		Declaration(parser);
	}

	parser.Consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}

void Synchronize(Parser& parser)
{
	parser.panicMode = false;

	while (parser.CurrentToken().m_oType != TOKEN_EOF)
	{
		if (parser.PreviousToken().m_oType == TOKEN_SEMICOLON)
			return;

		switch (parser.CurrentToken().m_oType.m_id) {
		case TOKEN_CLASS:
		case TOKEN_FUN:
		case TOKEN_VAR:
		case TOKEN_FOR:
		case TOKEN_IF:
		case TOKEN_WHILE:
		case TOKEN_PRINT:
		case TOKEN_RETURN:
			return;

		default:
			// Do nothing.
			;
		}

		parser.Advance();
	}
}

void NamedVariable(Parser& parser, Token name, bool can_assign)
{
	uint8_t get_op, set_op;
	int arg;
	arg = ResolveLocal(parser, parser.currentCompiler, name);
	if (arg != -1) {
	    get_op = OP_GET_LOCAL;
	    set_op = OP_SET_LOCAL;
	} else if ((arg = ResolveUpvalue(parser, parser.currentCompiler, name)) != -1) {
	    get_op = OP_GET_UPVALUE;
	    set_op = OP_SET_UPVALUE;
	} else {
		arg = parser.IdentifierConstant(name);
		get_op = OP_GET_GLOBAL;
		set_op = OP_SET_GLOBAL;
	}
	if (can_assign && parser.Match(TOKEN_EQUAL)) {
		Expression(parser);
		parser.EmitBytes(set_op, (uint8_t) arg);
	} else
		parser.EmitBytes(get_op, (uint8_t) arg);
}

void DeclareVariable(Parser& parser, Token name)
{
    if (parser.currentCompiler->scopeDepth == 0)
        return;

    for (int i = parser.currentCompiler->localCount - 1; i >= 0; i--) {
        Local *loc = &parser.currentCompiler->locals[i];
        if (loc->depth != -1 && loc->depth < parser.currentCompiler->scopeDepth) {
            break;
        }
        if (IdentifiersEqual(name, loc->name)) {
            parser.Error("Variable with this name already declared in this scope.");
        }
    }
    AddLocal(parser, name);
}

uint8_t ParseVariable(Parser& parser, Token& name, const char *msg)
{
    // parser.Consume(TOKEN_IDENTIFIER, msg);
	DeclareVariable(parser, name);

  	if (parser.currentCompiler->scopeDepth > 0)
		return 0;

	uint8_t name_id = parser.IdentifierConstant(name);
	// std::cout << parser.PreviousToken().GetText() << std::endl;
	// parser.Consume(TOKEN_COLON, "Expect ':'.");
	// parser.Consume(TOKEN_IDENTIFIER, "Expect variable type.");
    // DeclareVariable(parser);
    // if (parser.comp->scope_depth > 0)
    //     return (0);
    return name_id;
}

void DefineVariable(Parser& parser, uint8_t global)
{
    if (parser.currentCompiler->scopeDepth > 0) {
        parser.MarkInitialized();
        return;
    }
    parser.EmitBytes(OP_DEFINE_GLOBAL, global);
}

void VarDeclaration(Parser& parser, Token name)
{
    uint8_t global = ParseVariable(parser, name, "Expect variable name.");

    if (parser.Match(TOKEN_EQUAL)) {
        Expression(parser);
    } else {
        parser.EmitByte(OP_NIL);
    }
    parser.Consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    DefineVariable(parser, global);
}

void FuncDeclaration(Parser& parser, Token name)
{
	uint8_t global = ParseVariable(parser, name, "Expect function name.");
	parser.MarkInitialized();
	Function(parser, TYPE_FUNCTION, name);
	DefineVariable(parser, global);
}


int ResolveLocal(Parser& parser, Compiler *comp, Token& name)
{
    for (int i = comp->localCount - 1; i >= 0; i--) {
        Local* loc = &comp->locals[i];
        if (IdentifiersEqual(name, loc->name))
		{
            if (loc->depth == -1)
				parser.Error("Cannot read local variable in its own initializer.");
            return i;
        }
    }
    return -1;
}

int AddUpvalue(Parser& parser, Compiler *comp, uint8_t index, bool is_local)
{
    int upvalue_count = comp->function->upValueCount;
    for (int i = 0; i < upvalue_count; i++) {
        Upvalue *up = &comp->upvalues[i];
        if (up->index == index && up->isLocal == is_local) {
            return i;
        }
    }
    if (upvalue_count == UINT8_COUNT) {
        parser.Error("Too many closure variables in function.");
        return 0;
    }
    comp->upvalues[upvalue_count].isLocal = is_local;
    comp->upvalues[upvalue_count].index = index;
    return comp->function->upValueCount++;
}

int ResolveUpvalue(Parser& parser, Compiler *comp, Token& name)
{
    if (comp->enclosing == NULL)
        return -1;
    int local = ResolveLocal(parser, comp->enclosing, name);
    if (local != -1) {
        comp->enclosing->locals[local].isCaptured = true;
        return (AddUpvalue(parser, comp, (uint8_t) local, true));
    }
    int upvalue = ResolveUpvalue(parser, comp->enclosing, name);
    if (upvalue != -1) {
        return AddUpvalue(parser, comp, (uint8_t)upvalue, false);
    }
    return (-1);
}

void AddLocal(Parser& parser, Token name)
{
    if (parser.currentCompiler->localCount == UINT8_COUNT) {
        parser.Error("Too many local variables in function.");
        return;
    }

    Local *loc = &parser.currentCompiler->locals[parser.currentCompiler->localCount++];
    loc->name = name;
    loc->depth = -1;
    loc->isCaptured = false;
}

uint32_t Parser::hashString(const std::string& str)
{
  	uint32_t hash = 2166136261u;

	for (int i = 0; i < str.size(); i++) {
		hash ^= str[i];
		hash *= 16777619;
	}
  	return hash;
}

ObjectString* Parser::AllocateString(const std::string& str, uint32_t hash)
{
    // ObjectString* string = New<ObjectString>(*m_pAllocator, str);
    // ObjectString* string = Pointer<ObjectString>(str);
    ObjectString* string = new ObjectString(str);
    string->type = OBJ_STRING;
	string->hash = hash;

	if (m_pVm)
		m_pVm->strings.Set(string, NIL_VAL);

    return string;
}

ObjectString* Parser::CopyString(const std::string& value)
{
	uint32_t hash = hashString(value);
    ObjectString* interned = NULL;

	if (m_pVm)
		interned = m_pVm->strings.FindString(value, hash);

  	if (interned != NULL)
	  return interned;

    return AllocateString(value, hash);
}

ObjectString* Parser::TakeString(const std::string& value)
{
	uint32_t hash = hashString(value);
	ObjectString* interned = NULL;

	if (m_pVm)
		interned = m_pVm->strings.FindString(value, hash);
	if (interned != NULL)
		return interned;

    return AllocateString(value, hash);
}

uint8_t Parser::IdentifierConstant(const Token& name)
{
  	return MakeConstant(OBJ_VAL(CopyString(name.GetText())));
}

bool IdentifiersEqual(Token& a, Token& b)
{
	if (a.m_iLength != b.m_iLength)
		return false;
	return a.GetText() == b.GetText();
}

void Parser::BeginScope()
{
  	currentCompiler->scopeDepth++;
}

void Parser::EndScope()
{
  	currentCompiler->scopeDepth--;

	while (currentCompiler->localCount > 0 && currentCompiler->locals[currentCompiler->localCount - 1].depth > currentCompiler->scopeDepth) {
		if (currentCompiler->locals[currentCompiler->localCount - 1].isCaptured) {
	      EmitByte(OP_CLOSE_UPVALUE);
	    } else {
	      EmitByte(OP_POP);
	    }
		currentCompiler->localCount--;
	}
}

void Function(Parser& parser, FunctionType type, const Token& name)
{
	Compiler compiler(parser, type, name.GetText());
	parser.BeginScope();

	// Compile the parameter list.
	parser.Consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");
	if (!parser.IsToken(TOKEN_RIGHT_PAREN, false)) {
		do
		{
			parser.currentCompiler->function->arity++;
			if (parser.currentCompiler->function->arity > 255)
			{
				parser.ErrorAtCurrent("Cannot have more than 255 parameters.");
			}
			parser.Consume(TOKEN_IDENTIFIER, "Expect parameter name.");
			Token& name = (Token&) parser.PreviousToken();
			uint8_t paramConstant = ParseVariable(parser, name, "Expect parameter name.");
			DefineVariable(parser, paramConstant);
		} while (parser.Match(TOKEN_COMMA));
	}
	parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");

	// The body.
	parser.Consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
	Block(parser);

	// Create the function object.
	ObjectFunction* function = parser.EndCompiler();
	parser.EmitBytes(OP_CLOSURE, parser.MakeConstant(OBJ_VAL(function)));
	for (int i = 0; i < function->upValueCount; i++)
	{
	    parser.EmitByte(compiler.upvalues[i].isLocal ? 1 : 0);
	    parser.EmitByte(compiler.upvalues[i].index);
	}
}
