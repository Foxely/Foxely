
#include <cstring>
#include "Lexer.h"
#include "Token.h"
#include "Parser.h"
#include "object.hpp"
#include "gc.hpp"
#include "vm.hpp"
#include "statement.hpp"

Chunk* Parser::GetCurrentChunk()
{
	return &currentCompiler->function->chunk;
}

void Parser::SetCurrentChunk(Chunk& chunk)
{
  	compilingChunk = &chunk;
}

Parser::Parser()
{
	currentCompiler = NULL;
	currentClass = NULL;

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
    oLexer.Define(TOKEN_FUN, "func");
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
    // oLexer.DefineArea(TOKEN_SINGLE_COMMENT,'//', '\n');
    oLexer.Define(TOKEN_SINGLE_COMMENT,"//[^\n\r]*", true);
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
	m_pVm->Push(value);
	int constant = GetCurrentChunk()->AddConstant(value);
	m_pVm->Pop();

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


// -----------------------------------------
//		Rules				   		   	   -
// -----------------------------------------

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
    parser.Consume(TOKEN_IDENTIFIER, "Expect property name after '.'.");
    uint8_t name = parser.IdentifierConstant(parser.PreviousToken());

    if (can_assign && parser.Match(TOKEN_EQUAL)) {
        Expression(parser);
        parser.EmitBytes(OP_SET_PROPERTY, name);
    } else if (parser.Match(TOKEN_LEFT_PAREN)) {
        uint8_t arg_count = ArgumentList(parser);
        parser.EmitBytes(OP_INVOKE, name);
        parser.EmitByte(arg_count);
    } else {
        parser.EmitBytes(OP_GET_PROPERTY, name);
    }
}

void RuleThis(Parser& parser, bool can_assign)
{
    if (parser.currentClass == NULL) {
        parser.Error("Cannot use 'this' outside of a class.");
        return;
    }
   	Variable(parser, false);
}

void RuleSuper(Parser& parser, bool can_assign)
{
    if (parser.currentClass == NULL) {
        parser.Error("Cannot use 'super' outside of a class.");
    } else if (!parser.currentClass->hasSuperclass) {
        parser.Error("Cannot use 'super' in a class with no superclass.");
    }
    parser.Consume(TOKEN_DOT, "Expect '.' after 'super'.");
    parser.Consume(TOKEN_IDENTIFIER, "Expect superclass method name.");
    uint8_t name = parser.IdentifierConstant(parser.PreviousToken());
    NamedVariable(parser, Token("this", 4), false);
    if (parser.Match(TOKEN_LEFT_PAREN)) {
        uint8_t arg_count = ArgumentList(parser);
        NamedVariable(parser, Token("super", 5), false);
        parser.EmitBytes(OP_SUPER_INVOKE, name);
        parser.EmitByte(arg_count);
    } else {
        NamedVariable(parser, Token("super", 5), false);
        parser.EmitBytes(OP_GET_SUPER, name);
    }
}

void Variable(Parser& parser, bool can_assign)
{
	Token name = parser.PreviousToken();
	if (parser.IsToken(TOKEN_COLON) && parser.IsToken(TOKEN_VAR))
	{
		VarDeclaration(parser, name);
	}
	else if (parser.IsToken(TOKEN_DOUBLE_COLON))
	{
		if (parser.IsToken(TOKEN_FUN))
			FuncDeclaration(parser, name);
		else if (parser.IsToken(TOKEN_CLASS))
    		ClassDeclaration(parser, name);
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
	Statement(parser);
    if (parser.panicMode)
        Synchronize(parser);
}

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



// -----------------------------------------
//		Variable				   		   -
// -----------------------------------------

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
	DeclareVariable(parser, name);

  	if (parser.currentCompiler->scopeDepth > 0)
		return 0;

	uint8_t name_id = parser.IdentifierConstant(name);
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

/*
 * @brief Cette fonction permet de déclarer une nouvelle variable
 * @param parser c'est la ref vers le parser
 * @param name c'est le nom de la variable
*/
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



// -----------------------------------------
//		Function				   		   -
// -----------------------------------------

/*
 * @brief Cette fonction permet de déclarer une nouvelle fonction
 * @param parser c'est la ref vers le parser
 * @param name c'est le nom de la fonction
*/
void FuncDeclaration(Parser& parser, Token name)
{
	uint8_t global = ParseVariable(parser, name, "Expect function name.");
	parser.MarkInitialized();
	Function(parser, TYPE_FUNCTION, name);
	DefineVariable(parser, global);
}

/*
 * @brief Cette fonction permet d'initializer une fonction
 * @param parser c'est la ref vers le parser
 * @param type c'est le type de fonction (voir l'enum FunctionType)
 * @param name c'est le nom de la fonction
 * @return true si les deux sont égaux sinon false dans le cas contraire
*/
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



// -----------------------------------------
//		UpValue				   			   -
// -----------------------------------------

/*
 * @brief Cette fonction permet de hasher la string passé en paramètre
 * @param parser c'est la ref vers le parser
 * @param comp c'est la ref vers un compiler
 * @param index c'est la position de la variable dans le tableau des variables locales
 * @param is_local ça permet de savoir si la variable est local ou pas
 * @return un index qui correspond à la position de la variable local dans la upvalue
*/
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

/*
 * @brief Cette fonction permet de chercher dans la liste du block au dessus le nom de la variable local correspondant et résoudre les problèmes comme l'initialisation et le rajouter dans la liste des upvalues du block actuel
 * @param parser c'est la ref vers le parser
 * @param comp c'est la ref vers un compiler
 * @param name c'est le nom de la variable
 * @return un index qui correspond à la position de la variable local dans la upvalue
 * @note:
		// Block1
		{
			a: var;		<- Variable Local
			
			// Block 2
			{
				b: var;		<- Variable Local

				// Block 3
				{
					print a; 		<- UpValue
				}
			}
		}

		Voila ce qui se passe en interne

		Block1:
			locals:
				- a
			upvalues:
			Block2:
				locals:
					- b
				upvalues:
					- a
				Block 3:
					locals:
					upvalues:
						- a
						- b
	Comme ça on peut accéder à la variable 'a' dans le Block3 facilement
*/
int ResolveUpvalue(Parser& parser, Compiler *comp, Token& name)
{
    if (comp->enclosing == NULL)
        return -1;
	// Chercher dans la liste des variables locales
    int local = ResolveLocal(parser, comp->enclosing, name);
    if (local != -1) {
        comp->enclosing->locals[local].isCaptured = true;
        return (AddUpvalue(parser, comp, (uint8_t) local, true));
    }
	// Si la variable n'est pas local au block actuel, chercher dans le block au-dessus et l'enregistrer en tant que upvalue
    int upvalue = ResolveUpvalue(parser, comp->enclosing, name);
    if (upvalue != -1) {
        return AddUpvalue(parser, comp, (uint8_t)upvalue, false);
    }
	// Sinon on n'a rien trouvé, la variable n'existe pas
    return (-1);
}


// -----------------------------------------
//		Local Variable				   	   -
// -----------------------------------------

/*
 * @brief Cette fonction permet de chercher dans la liste le nom de la variable local correspondant et résoudre les problèmes comme l'initialisation
 * @param parser c'est la ref vers le parser
 * @param comp c'est la ref vers un compiler
 * @param name c'est le nom de la variable
 * @return un index qui correspond à la position de la variable local
*/
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

/*
 * @brief Cette fonction permet d'ajouter une variable locale
 * @param parser c'est la ref vers le parser
 * @param name c'est le nom de la variable
*/
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

// -----------------------------------------
//		String Manipulation				   -
// -----------------------------------------

/*
* @brief Cette fonction permet de hasher la string passé en paramètre
* @param str la chaine de caractère qui sera hasher
* @return un nombre unique qui correspond à la position de la string dans le tableau
* @note Hasher veut dire produire un identifiant unique crypté
*/
uint32_t Parser::hashString(const std::string& str)
{
	uint32_t hash = 2166136261u;

	for (int i = 0; i < str.size(); i++) {
		hash ^= str[i];
		hash *= 16777619;
	}
	return hash;
}


/*
* @brief Cette fonction permet de transformer la string passé en param en ObjectString compréhensible par le langage
* @param str la chaine de caractère qui sera stocker
* @param hash l'identifiant unique de la string après un hashage (passage dans une fonction de hashage comme 'hashString')
* @return un pointeur ObjectString alloué dans le garbage Collector
*/
ObjectString* Parser::AllocateString(const std::string& str, uint32_t hash)
{
	ObjectString* string = new ObjectString(str);
	string->type = OBJ_STRING;
	string->hash = hash;

	m_pVm->Push(OBJ_VAL(string));
	m_pVm->strings.Set(string, NIL_VAL);
	m_pVm->Pop();
	return string;
}

/*
* @brief Cette fonction permet de copier une string passé en param et return un ObjectString compréhensible par le langage
* @param value la chaine de caractère qui sera copier
* @return une copie de la string sous un pointeur ObjectString alloué dans le garbage Collector
*/
ObjectString* Parser::CopyString(const std::string& value)
{
	uint32_t hash = hashString(value);
	ObjectString* interned = NULL;

	interned = m_pVm->strings.FindString(value, hash);

	if (interned != NULL)
		return interned;

	return AllocateString(value, hash);
}

/*
* @brief Cette fonction fait la même chose que 'CopyString'
*/
ObjectString* Parser::TakeString(const std::string& value)
{
	uint32_t hash = hashString(value);
	ObjectString* interned = NULL;

	interned = m_pVm->strings.FindString(value, hash);
	if (interned != NULL)
		return interned;

	return AllocateString(value, hash);
}

/*
 * @brief Cette fonction permet de créer un nombre unique pour le nom de l'identifer passer en paramètre 
 * @param name le Token qui contient le nom de l'identifier
 * @return un index de l'emplacement où est stocker l'identifier
 * @note test::func() { ... }: test est un Identifer
 *		 a: var; : a est un Identifer
*/
uint8_t Parser::IdentifierConstant(const Token& name)
{
  	return MakeConstant(OBJ_VAL(CopyString(name.GetText())));
}

/*
 * @brief Cette fonction permet de comparer le nom de deux Identifiers
 * @param a le Token qui sera comparé
 * @param b le Token qui sera comparé
 * @return true si les deux sont égaux sinon false dans le cas contraire
*/
bool IdentifiersEqual(Token& a, Token& b)
{
	if (a.m_iLength != b.m_iLength)
		return false;
	return a.GetText() == b.GetText();
}

/*
 * @brief Cette fonction permet d'incrementer la scope
*/
void Parser::BeginScope()
{
  	currentCompiler->scopeDepth++;
}

/*
 * @brief Cette fonction permet d'décrementer la scope
*/
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

void Method(Parser& parser)
{
	parser.Consume(TOKEN_IDENTIFIER, "Expect method name.");
	uint8_t constant = parser.IdentifierConstant(parser.PreviousToken());
	FunctionType type = TYPE_METHOD;
	if (parser.PreviousToken().GetText() == "init") {
		type = TYPE_INITIALIZER;
	}
	Function(parser, type, parser.PreviousToken());
	parser.EmitBytes(OP_METHOD, constant);
}

void ClassDeclaration(Parser& parser, Token& name)
{
	uint8_t nameConstant = parser.IdentifierConstant(name);
	DeclareVariable(parser, name);

	parser.EmitBytes(OP_CLASS, nameConstant);
	DefineVariable(parser, nameConstant);

	ClassCompiler classCompiler(name);
	classCompiler.enclosing = parser.currentClass;
	classCompiler.hasSuperclass = false;
	parser.currentClass = &classCompiler;

	if (parser.Match(TOKEN_LESS))
	{
		parser.Consume(TOKEN_IDENTIFIER, "Expect superclass name.");
		Variable(parser, false);
		Token superName = parser.PreviousToken();
		if (IdentifiersEqual(name, superName)) {
			parser.Error("A class cannot inherit from itself.");
		}
		parser.BeginScope();
		AddLocal(parser, Token("super", 5));
		DefineVariable(parser, 0);
		NamedVariable(parser, name, false);
		parser.EmitByte(OP_INHERIT);
		classCompiler.hasSuperclass = true;
	}

	NamedVariable(parser, name, false);

	parser.Consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");
	while (!parser.PeekTokenIsType(TOKEN_RIGHT_BRACE) && !parser.PeekTokenIsType(TOKEN_EOF)) {
		Method(parser);
	}
	parser.Consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");
	parser.EmitByte(OP_POP);

	if (classCompiler.hasSuperclass) {
		parser.EndScope();
	}

	parser.currentClass = parser.currentClass->enclosing;
}

// ----------------------------
//      Array
// ----------------------------

// static void List(Parser& parser, bool canAssign)
// {
//     parser.EmitByte(OP_NEW_LIST);

//     do
//     {
//         if (parser.PeekNextTokenIsType(TOKEN_RIGHT_BRACKET))
//             break;

//         Expression(parser);
//         parser.EmitByte(OP_ADD_LIST);
//     }
//     while (parser.Match(TOKEN_COMMA));

//     parser.Consume(TOKEN_RIGHT_BRACKET, "Expected closing ']'");
// }