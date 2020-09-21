
#include <cstring>
#include "Parser.h"
#include "compiler.h"
#include "object.hpp"

Chunk* Parser::GetCurrentChunk()
{
  	return compilingChunk;
}

void Parser::SetCurrentChunk(Chunk& chunk)
{
  	compilingChunk = &chunk;
}

Parser::Parser()
{
    oLexer.Define(TOKEN_NEW_LINE,"\n", true);
    oLexer.Define(TOKEN_WS,"[ \t\r\b]+", true);
    oLexer.Define(TOKEN_NUMBER,"[0-9]+");
    oLexer.Define(TOKEN_STRING,"\".*\"");
	oLexer.Define(TOKEN_PLUS, "\\+");
	oLexer.Define(TOKEN_MINUS, "-");
    // oLexer.Define("Double Colon",":{2}");
    // oLexer.Define("Single Colons",":{1}");
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
    // oLexer.Define("SingleComment","//.*\n");
    // oLexer.Define("MultiComment","/\\*.*\\*/");
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
}

void Parser::Advance()
{
    oPreviousToken = CurrentToken();
    for (;;) {
        NextToken();
        if (CurrentToken().m_oType != 84)
            break;
        ErrorAtCurrent(CurrentToken().GetText().c_str());
    }
}

// void Parser::MarkInitialized()
// {
//     if (comp->scope_depth == 0)
//         return;
//     comp->locals[comp->local_count - 1].depth = comp->scope_depth;
// }

bool Compile(Parser& parser, const std::string &strText, Chunk* chunk)
{
    if (!parser.Init(strText))
        return false;
	parser.compilingChunk = chunk;
    parser.hadError = false;
    parser.panicMode = false;
    // while (!parser.Match(TOKEN_EOF)) {
    //     Declaration(parser);
    // }
    // ObjectFunction *func = parser.EndCompiler();
    // parser.Advance();
    Expression(parser);
    parser.Consume(TOKEN_EOF, "Expect end of expression.");
    parser.EndCompiler();
    return !parser.hadError;
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
    compilingChunk->WriteChunk(byte, PreviousToken().m_iLinesTraversed);
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

uint8_t Parser::MakeConstant(Value value)
{
    
	int constant = compilingChunk->AddConstant(value);
	if (constant > UINT8_MAX) {
		Error("Too many constants in one chunk.");
		return 0;
	}
  	return (uint8_t) constant;
}

void Parser::EndCompiler()
{
    std::cout << "END" << std::endl;
	EmitByte(OP_RETURN);
    // ObjectFunction *func = comp->function;
    // #ifdef DEBUG_PRINT_CODE
    // if (!hadError) {
    //     disassemble_chunk(compilingChunk, func->name != NULL ? func->name->chars : "<script>");
    // }
    // #endif
    // comp = comp->enclosing;
    // return (func);
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
    while (!parser.CurrentToken().GetText().empty() && preced <= parser.GetRule(parser.CurrentToken().m_oType.m_id)->precedence) {
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
    std::cout << "Number------" << std::endl;
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
    int operatorType = parser.CurrentToken().m_oType.m_id;
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
    // int end_jump = parser.EmitJump(OP_JUMP_IF_FALSE);

    // parser.EmitByte(OP_POP);
    // ParsePrecedence(PREC_AND);
    // PatchJump(end_jump);
}

void RuleOr(Parser& parser, bool can_assign)
{
    // int else_jump = EmitJump(OP_JUMP_IF_FALSE);
    // int end_jump = EmitJump(OP_JUMP);

    // PatchJump(else_jump);
    // EmitByte(OP_POP);

    // ParsePrecedence(PREC_OR);
    // PatchJump(end_jump);
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
    // NamedVariable(parser, parser.CurrentToken(), can_assign);
}

void String(Parser& parser, bool can_assign)
{
    // EmitConstant(parser, OBJ_VAL(CopyString(parser->vm, parser->previous.start + 1, parser->previous.length - 2)));
}

void CallCompiler(Parser& parser, bool can_assign)
{
    // uint8_t arg_count = ArgumentList();
    // EmitBytes(OP_CALL, arg_count);
}

// void Declaration(Parser& parser)
// {
//     if (parser.Match(TOKEN_CLASS)) {
//         // ClassDeclaration(parser);
//     } else if (parser.Match(TOKEN_FUN)) {
//         // FunctionDeclaration(parser);
//     } else {
//         if (parser.Match(TOKEN_VAR)) {
//             VarDeclaration(parser);
//         } else
//             Statement(parser);
//     }
//     if (parser.panicMode)
//         Synchronize();
// }


// void NamedVariable(Parser& parser, Token name, bool can_assign)
// {
//     uint8_t get_op, set_op;
//     int arg = ResolveLocal(parser, parser.comp, &name);
//     if (arg != -1) {
//         get_op = OP_GET_LOCAL;
//         set_op = OP_SET_LOCAL;
//     } else if ((arg = ResolveUpvalue(parser, parser.comp, &name)) != -1) {
//         get_op = OP_GET_UPVALUE;
//         set_op = OP_SET_UPVALUE;
//     } else {
//         arg = IdentifierConstant(parser, &name);
//         get_op = OP_GET_GLOBAL;
//         set_op = OP_SET_GLOBAL;
//     }
//     if (can_assign && parser.Match(TOKEN_EQUAL)) {
//         Expression(parser);
//         parser.EmitBytes(set_op, (uint8_t) arg);
//     } else
//         parser.EmitBytes(get_op, (uint8_t) arg);
// }

// void DeclareVariable(Parser& parser)
// {
//     if (parser.comp->scope_depth == 0)
//         return;

//     Token name = parser.PreviousToken();
//     for (int i = parser.comp->local_count - 1; i >= 0; i--) {
//         Local *loc = &parser.comp->locals[i];
//         if (loc->depth != -1 && loc->depth < parser.comp->scope_depth) {
//             break;
//         }
//         if (IdentifiersEqual(name, &loc->name)) {
//             parser.Error("Variable with this name already declared in this scope.");
//         }
//     }
//     AddLocal(parser, *name);
// }

// uint8_t ParseVariable(Parser& parser, const char *msg)
// {
//     parser.Consume(TOKEN_IDENTIFIER, msg);
//     DeclareVariable(parser);
//     if (parser.comp->scope_depth > 0)
//         return (0);
//     return IdentifierConstant(parser, &parser.PreviousToken());
// }

// void DefineVariable(Parser& parser, uint8_t global)
// {
//     if (parser.comp->scope_depth > 0) {
//         parser.MarkInitialized();
//         return;
//     }
//     parser.EmitBytes(OP_DEFINE_GLOBAL, global);
// }

// void VarDeclaration(Parser& parser)
// {
//     uint8_t global = ParseVariable(parser, "Expect variable name.");

//     if (parser.Match(TOKEN_EQUAL)) {
//         Expression(parser);
//     } else {
//         parser.EmitByte(OP_NIL);
//     }
//     parser.Consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
//     DefineVariable(parser, global);
// }


// int ResolveLocal(Parser& parser, Compiler *comp, Token *name)
// {
//     for (int i = comp->local_count - 1; i >= 0; i--) {
//         Local* loc = &comp->locals[i];
//         if (IdentifiersEqual(name, &loc->name)) {
//             loc->depth == -1 ? parser.Error("Cannot read local variable in its own initializer.") : 0;
//             return i;
//         }
//     }
//     return -1;
// }

// int AddUpvalue(Parser& parser, Compiler *comp, uint8_t index, bool is_local)
// {
//     int upvalue_count = comp->function->upValue_count;
//     for (int i = 0; i < upvalue_count; i++) {
//         Upvalue *up = &comp->upvalues[i];
//         if (up->index == index && up->is_local == is_local) {
//             return i;
//         }
//     }
//     if (upvalue_count == UINT8_COUNT) {
//         parser.Error("Too many closure variables in function.");
//         return 0;
//     }
//     comp->upvalues[upvalue_count].is_local = is_local;
//     comp->upvalues[upvalue_count].index = index;
//     return comp->function->upValue_count++;
// }

// int ResolveUpvalue(Parser& parser, Compiler *comp, Token *name)
// {
//     if (comp->enclosing == NULL)
//         return -1;
//     int local = ResolveLocal(parser, comp->enclosing, name);
//     if (local != -1) {
//         comp->enclosing->locals[local].is_captured = true;
//         return (AddUpvalue(parser, comp, (uint8_t) local, true));
//     }
//     int upvalue = ResolveUpvalue(parser, comp->enclosing, name);
//     if (upvalue != -1) {
//         return AddUpvalue(parser, comp, (uint8_t)upvalue, false);
//     }
//     return (-1);
// }

// void AddLocal(Parser& parser, Token name)
// {
//     if (parser.comp->local_count == UINT8_COUNT) {
//         parser.Error("Too many local variables in function.");
//         return;
//     }
//     Local *loc = &parser.comp->locals[parser.comp->local_count++];
//     loc->name = name;
//     loc->depth = -1;
//     loc->is_captured = false;
// }