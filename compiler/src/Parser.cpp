
#include <cstring>
#include "Lexer.h"
#include "Token.h"
#include "Parser.h"
#include "statement.hpp"
#include "LinearAllocator.h"

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
    oLexer.Define(TOKEN_REF, "&");
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
    oLexer.Define(TOKEN_NEW, "new");
    oLexer.Define(TOKEN_EQUAL, "=");
    oLexer.Define(TOKEN_EQUAL_EQUAL, "==");
    oLexer.Define(TOKEN_SHEBANG,"#[^\n\r]*", true);
    oLexer.Define(TOKEN_RIGHT_BRACKET,"\\[");
    oLexer.Define(TOKEN_LEFT_BRACKET,"\\]");
    oLexer.Define(TOKEN_IDENTIFIER,"[A-Za-z_]+[0-9]*");
    // oLexer.Define("Arrow","->");
    oLexer.Define(TOKEN_SINGLE_COMMENT,"//[^\n\r]*", true);
    oLexer.Define(TOKEN_MULTI_COMMENT,"/\\*.+\\*/", true);
    oLexer.Define(TOKEN_EOF,"[\0]");


    rules[TOKEN_NEW] = { NewRule, NULL, PREC_NONE };
    rules[TOKEN_REF] = { RefVariable, NULL, PREC_NONE };
    rules[TOKEN_RIGHT_BRACKET] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_LEFT_BRACKET] = { NULL, NULL, PREC_NONE };
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
    rules[TOKEN_AND] = { NULL, RuleAnd, PREC_AND };
    rules[TOKEN_CLASS] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_ELSE] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_FALSE] = { Literal, NULL, PREC_NONE };
    rules[TOKEN_FOR] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_FUN] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_IF] = { NULL, NULL, PREC_NONE };
    rules[TOKEN_NIL] = { Literal, NULL, PREC_NONE };
    rules[TOKEN_OR] = { NULL, RuleOr, PREC_OR };
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

bool CreateAst(Parser& parser, const std::string& strText)
{
    if (!parser.Init(strText))
    {
        std::cout << "Empty file\n";
        return false;
    }

    if (VERBOSE_TOKEN) {
        std::cout << "======= Tokens =========\n";
        helper::Dump(parser.GetLexer());
        std::cout << "========================\n";
    }

    parser.hadError = false;
    parser.panicMode = false;

	while (!parser.IsEnd()) {
        AstNode* node = nullptr;
		Declaration(node, parser);
        if (node)
            parser.pRoot.push_back(node);
	}
    return !parser.hadError;
}

void Expression(AstNode*& out, Parser& parser)
{
    ParsePrecedence(out, parser, PREC_ASSIGNMENT);
}

void Parser::Consume(int oType, const char* message)
{
	if (!IsToken(oType)) {
		ErrorAtCurrent(message);
	}
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

void ParsePrecedence(AstNode*& out, Parser& parser, Precedence preced)
{
    parser.Advance();
    ParseRule* pRule = parser.GetRule(parser.PreviousToken().m_oType.m_id);
    parse_fn prefix_rule = pRule ? pRule->prefix : nullptr;
    if (prefix_rule == NULL) {
        parser.Error("Expect expression.");
        return;
    }
    bool can_assign = preced <= PREC_ASSIGNMENT;
    prefix_rule(out, parser, can_assign);
	pRule = parser.GetRule(parser.CurrentToken().m_oType.m_id);
    while (!parser.hadError && pRule && preced <= pRule->precedence && !parser.PreviousToken().GetText().empty()) {
        parser.Advance();
        parse_fn infix_rule = parser.GetRule(parser.PreviousToken().m_oType.m_id)->infix;
		if (infix_rule != NULL)
        	infix_rule(out, parser, can_assign);
		pRule = parser.GetRule(parser.CurrentToken().m_oType.m_id);
    }

    if (can_assign && parser.Match(TOKEN_EQUAL)) {
        parser.Error("Invalid assignment target.");
    }
}


// -----------------------------------------
//		Rules				   		   	   -
// -----------------------------------------

void Number(AstNode*& out, Parser& parser, bool can_assign)
{
	// double value = strtod(parser.PreviousToken().GetText().c_str(), NULL);
    // std::cout << parser.PreviousToken().GetText();
    out = New<Type>(*parser.allocator, parser.PreviousToken().GetText());
	// parser.EmitConstant(NUMBER_VAL(value));
}

void Grouping(AstNode*& out, Parser& parser, bool can_assign)
{
  	Expression(out, parser);
  	parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Unary(AstNode*& out, Parser& parser, bool can_assign)
{
    int operator_type = parser.PreviousToken().m_oType.m_id;
    ParsePrecedence(out, parser, PREC_UNARY);
    switch (operator_type) {
        case TOKEN_BANG:
            // parser.EmitByte(OP_NOT);
            break;
        case TOKEN_MINUS:
            // parser.EmitByte(OP_NEGATE);
            break;
        default:
        return;
    }
}


void Binary(AstNode*& out, Parser& parser, bool can_assign)
{
    int operatorType = parser.PreviousToken().m_oType.m_id;
    ParseRule *rule = parser.GetRule(operatorType);
    AstNode* num2 = NULL;
    ParsePrecedence(num2, parser, (Precedence)(rule->precedence + 1));

    switch (operatorType) {
        case TOKEN_BANG_EQUAL:
        {
            out = New<BangEqualAstExpression>(*parser.allocator, out, num2);
            break;
        }
        case TOKEN_EQUAL_EQUAL:
        {
            out = New<EqualEqualAstExpression>(*parser.allocator, out, num2);
            break;
        }
        case TOKEN_GREATER:
        {
            out = New<GreaterAstExpression>(*parser.allocator, out, num2);
            break;
        }
        case TOKEN_GREATER_EQUAL:
        {
            out = New<GreaterEqualAstExpression>(*parser.allocator, out, num2);
            break;
        }
        case TOKEN_LESS:
        {
            out = New<LessAstExpression>(*parser.allocator, out, num2);
            break;
        }
        case TOKEN_LESS_EQUAL:
        {
            out = New<LessEqualAstExpression>(*parser.allocator, out, num2);
            break;
        }
        case TOKEN_PLUS:
        {
            out = New<SumAstExpression>(*parser.allocator, out, num2);
            break;
        }
        case TOKEN_MINUS:
        {
            out = New<SubtractionAstExpression>(*parser.allocator, out, num2);
            break;
        }
        case TOKEN_STAR:
        {
            out = New<MultiplicationAstExpression>(*parser.allocator, out, num2);
            break;
        }
        case TOKEN_SLASH:
        {
            out = New<DivisionAstExpression>(*parser.allocator, out, num2);
            break;
        }
        default:
            return;
    }
}

void Literal(AstNode*& out, Parser& parser, bool can_assign)
{
    int operatorType = parser.PreviousToken().m_oType.m_id;
    switch (operatorType) {
        case TOKEN_FALSE:
            // parser.EmitByte(OP_FALSE);
            out = New<Type>(*parser.allocator, parser.PreviousToken().GetText());
            break;
        case TOKEN_NIL:
            // parser.EmitByte(OP_NIL);
            out = New<Type>(*parser.allocator, parser.PreviousToken().GetText());
            break;
        case TOKEN_TRUE:
            // parser.EmitByte(OP_TRUE);
            out = New<Type>(*parser.allocator, parser.PreviousToken().GetText());
            break;
        default:
        return;
    }
}

void RuleAnd(AstNode*& out, Parser& parser, bool can_assign)
{
    ParsePrecedence(out, parser, PREC_AND);
}

void RuleOr(AstNode*& out, Parser& parser, bool can_assign)
{
    ParsePrecedence(out, parser, PREC_OR);
}

void Dot(AstNode*& out, Parser& parser, bool can_assign)
{
    AstDot* dot = New<AstDot>(*parser.allocator);
    Expression(dot->expression, parser);
    // parser.Consume(TOKEN_IDENTIFIER, "Expect property name after '.'.");
    // uint8_t name = parser.IdentifierConstant(parser.PreviousToken());
    // std::string name = parser.PreviousToken().GetText();

    // if (can_assign && parser.Match(TOKEN_EQUAL)) {
    //     // Expression(dot->expression, parser);
    //     // parser.EmitBytes(OP_SET_PROPERTY, name);
    // } else if (parser.Match(TOKEN_LEFT_PAREN)) {
    //     AstFuncCall* funcCall = New<AstFuncCall>(*parser.allocator, name);
    //     funcCall->declarationList = ArgumentsList(parser);
    //     dot->expression = funcCall;
    //     // uint8_t arg_count = ArgumentList(out, parser);
    //     // parser.EmitBytes(OP_INVOKE, name);
    //     // parser.EmitByte(arg_count);
    // } else {
    //     AstVar* variable = New<AstVar>(*parser.allocator, name);
    //     dot->expression = variable;
    //     // parser.EmitBytes(OP_GET_PROPERTY, name);
    // }
    // parser.pRoot.push_back(dot);
    out = dot;
}

void RuleThis(AstNode*& out, Parser& parser, bool can_assign)
{
    if (parser.currentClass == NULL) {
        parser.Error("Cannot use 'this' outside of a class.");
        return;
    }

    // NamedVariable(parser, Token("this", 4), false);
    if (parser.Match(TOKEN_DOT))
    {
        parser.Consume(TOKEN_IDENTIFIER, "Expect class method name.");
        // uint8_t name = parser.IdentifierConstant(parser.PreviousToken());

        if (parser.Match(TOKEN_LEFT_PAREN)) {
            uint8_t arg_count = ArgumentList(out, parser);
            // parser.EmitBytes(OP_INVOKE, name);
            // parser.EmitByte(arg_count);
        } else if (parser.Match(TOKEN_EQUAL)) {
            Expression(out, parser);
            // parser.EmitBytes(OP_SET_PROPERTY, name);
        }
        else
        {
            // parser.EmitBytes(OP_GET_PROPERTY, name);
        }
    }
}

void RuleSuper(AstNode*& out, Parser& parser, bool can_assign)
{
    if (parser.currentClass == NULL) {
        parser.Error("Cannot use 'super' outside of a class.");
    } else if (!parser.currentClass->hasSuperclass) {
        parser.Error("Cannot use 'super' in a class with no superclass.");
    }
    parser.Consume(TOKEN_DOT, "Expect '.' after 'super'.");
    parser.Consume(TOKEN_IDENTIFIER, "Expect superclass method name.");
    // uint8_t name = parser.IdentifierConstant(parser.PreviousToken());
    // NamedVariable(parser, Token("this", 4), false);
    if (parser.Match(TOKEN_LEFT_PAREN)) {
        uint8_t arg_count = ArgumentList(out, parser);
        // NamedVariable(parser, Token("super", 5), false);
        // parser.EmitBytes(OP_SUPER_INVOKE, name);
        // parser.EmitByte(arg_count);
    } else {
        // NamedVariable(parser, Token("super", 5), false);
        // parser.EmitBytes(OP_GET_SUPER, name);
    }
}

std::list<AstNode*> ArgumentsList(Parser& parser)
{
	std::list<AstNode*> pRoot;

	if (!parser.IsToken(TOKEN_RIGHT_PAREN, false)) {
		do
		{
            AstNode* out = NULL;
			Expression(out, parser);
            pRoot.push_back(out);
		} while (parser.Match(TOKEN_COMMA));
	}
	parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
	return pRoot;
}

void NewRule(AstNode*& out, Parser& parser, bool can_assign)
{
    AstNew* astNew = New<AstNew>(*parser.allocator);
    out = astNew;

    parser.Consume(TOKEN_IDENTIFIER, "Expect identifier after 'new'.");
    astNew->objectName = parser.PreviousToken().GetText();
	parser.Consume(TOKEN_LEFT_BRACE, "Expect '{' class name.");
	parser.Consume(TOKEN_RIGHT_BRACE, "Expect '}' after arguments.");
}

void RefVariable(AstNode*& out, Parser& parser, bool can_assign)
{
    parser.Consume(TOKEN_IDENTIFIER, "Expect identifier after '&'.");
	Token name = parser.PreviousToken();
	if (parser.IsToken(TOKEN_COLON))
		parser.Error("Passing reference to a variable declaration is not accepted.");
	else if (parser.IsToken(TOKEN_DOUBLE_COLON))
		parser.Error("Passing reference to a function or class declaration is not accepted.");
	else
    {
        AstVar* variable = New<AstVar>(*parser.allocator, name.GetText(), true);
        out = variable;
        while (parser.IsToken("."))
        {
            AstNode* dot = NULL;
            Dot(dot, parser, false);
            variable->children.push_back(dot);
        }
    }
}

void Variable(AstNode*& out, Parser& parser, bool can_assign)
{
	Token name = parser.PreviousToken();
	if (parser.IsToken(TOKEN_COLON))
		VarDeclaration(out, parser, name);
	else if (parser.IsToken(TOKEN_DOUBLE_COLON))
	{
		if (parser.IsToken(TOKEN_FUN))
			FuncDeclaration(out, parser, name);
		else if (parser.IsToken(TOKEN_CLASS))
    		ClassDeclaration(out, parser, name);
	}
	else
    {
        if (parser.IsToken("("))
        {
            AstFuncCall* func = New<AstFuncCall>(*parser.allocator, name.GetText());
            func->declarationList = ArgumentsList(parser);
            out = func;
        }
        else
        {
            AstVar* variable = New<AstVar>(*parser.allocator, name.GetText());
            out = variable;
            while (parser.IsToken("."))
            {
                AstNode* dot = NULL;
                Dot(dot, parser, false);
                variable->children.push_back(dot);
            }

            if (parser.Match(TOKEN_EQUAL))
            {
                variable->isAssign = true;
                Expression(variable->expression, parser);
            }
        }
    }
}

void String(AstNode*& out, Parser& parser, bool can_assign)
{
    out = New<Type>(*parser.allocator, "\"" + parser.PreviousToken().GetText() + "\"");
}

uint8_t ArgumentList(AstNode*& out, Parser& parser)
{
	uint8_t argCount = 0;
	if (!parser.IsToken(TOKEN_RIGHT_PAREN, false)) {
		do
		{
			Expression(out, parser);
			if (argCount == 255)
				parser.Error("Cannot have more than 255 arguments.");
			argCount++;
		} while (parser.Match(TOKEN_COMMA));
	}
	parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
	return argCount;
}

void CallCompiler(AstNode*& out, Parser& parser, bool can_assign)
{
    // uint8_t arg_count = ArgumentList(out, parser);
    // parser.EmitBytes(OP_CALL, arg_count);
    std::cerr << "bhbh";
}

void Declaration(AstNode*& out, Parser& parser)
{
	Statement(out, parser);
    if (parser.panicMode)
        Synchronize(parser);
}

void Block(AstNode*& out, Parser& parser)
{
	while (!parser.IsToken(TOKEN_RIGHT_BRACE, false) && !parser.IsToken(TOKEN_EOF, false)) {
		Declaration(out, parser);
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

/*
 * @brief Cette fonction permet de déclarer une nouvelle variable
 * @param parser c'est la ref vers le parser
 * @param name c'est le nom de la variable
*/
void VarDeclaration(AstNode*& out, Parser& parser, Token name)
{
    AstVarDeclaration* variable = ParseVarDeclarationBody(parser, name);
    if (parser.Match(TOKEN_EQUAL)) {
        variable->assign = true;
        Expression(variable->expression, parser);
    }
    out = variable;
    parser.Consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
}


// -----------------------------------------
//		Function				   		   -
// -----------------------------------------

Type* ParseTypeUsage(Parser& parser)
{
    Type *pType = nullptr, *pStructDeclaration = nullptr, *pUnionDeclaration = nullptr;
    int iPtrCount = 0;
    Token oTypeName;

    for (; parser.IsToken("*"); ++iPtrCount);
    Token oClass;
    if (parser.IsToken(TOKEN_IDENTIFIER, "as") && parser.IsTokenThenAssign(TOKEN_IDENTIFIER, oClass)) {
        // pStructDeclaration = ParseEntityBody(oClass, Token());
    } else {
        if (!parser.IsTokenThenAssign(TOKEN_IDENTIFIER, oTypeName)) {
            parser.ErrorAtCurrent("Missing type name.");
            return nullptr;
        }
    }
    
    pType = New<Type>(*parser.allocator, oTypeName.GetText());
    pType->m_pointerCount = iPtrCount;

    // AstNode** pArraySizeTarget = &pType->TypeUsage.m_pFirstArraySizeExpression;

    // for(;;)
    // {
    //     if(IsToken("["))
    //     {
    //         *pArraySizeTarget = ParseExpression();
    //         pArraySizeTarget = &(*pArraySizeTarget)->m_pNext;

    //         if(!IsToken("]"))
    //         {
    //             PushError("Missing ]");
    //             break;
    //         }
    //     }
    //     else
    //         break;
    // }
    return pType;
}


/*
 * @brief Cette fonction permet de déclarer une nouvelle fonction
 * @param parser c'est la ref vers le parser
 * @param name c'est le nom de la fonction
*/
void FuncDeclaration(AstNode*& out, Parser& parser, Token name)
{
	// uint8_t global = ParseVariable(parser, name, "Expect function name.");
	// parser.MarkInitialized();
    out = ParseFunction(parser, name);

	// DefineVariable(parser, global);
}

AstDeclaration* ParseDeclarationBody(Parser& parser, const Token& oName)
{
    AstDeclaration *pRoot = New<AstDeclaration>(*parser.allocator, oName.GetText());
    pRoot->type = ParseTypeUsage(parser);
    return pRoot;
}

AstVarDeclaration* ParseVarDeclarationBody(Parser& parser, const Token& oName)
{
    AstVarDeclaration *pRoot = New<AstVarDeclaration>(*parser.allocator, oName.GetText());
    pRoot->type = ParseTypeUsage(parser);
    return pRoot;
}

std::list<AstNode*> ParseDeclarationList(Parser& parser)
{
    std::list<AstNode*> pRoot;
    Token name = Token();

    for (Token curr = parser.CurrentToken(); ; curr = parser.CurrentToken())
    {
        if (parser.GetLexer().TokenMatch(curr, "}") || parser.GetLexer().TokenMatch(curr, ")"))
            break;
        // ParseTagList();
        if (parser.IsTokenThenAssign(TOKEN_IDENTIFIER, name) && parser.IsToken(":")) {
            AstDeclaration* declaration = ParseDeclarationBody(parser, name);
            pRoot.push_back(declaration);
            if (!parser.GetLexer().TokenMatch(parser.CurrentToken(), "}") && !parser.GetLexer().TokenMatch(parser.CurrentToken(), ")"))
            {
                if (!parser.IsToken(";") && !parser.IsToken(","))
                {
                    parser.ErrorAtCurrent("Expected ';' or ',' after declaration.");
                    break;
                }
            }
        }
        else {
            parser.ErrorAtCurrent("Expected declaration (<name> : <type>).");
            break;
        }
    }
    return pRoot;
}

std::list<AstNode*> ParseFunctionBody(Parser& parser)
{
    std::list<AstNode*> pRoot;

    for (Token curr = parser.CurrentToken(); ; curr = parser.CurrentToken())
    {
        if (parser.GetLexer().TokenMatch(curr, "}") || parser.GetLexer().TokenMatch(curr, ")"))
            break;
        AstNode* declaration = NULL;
        Statement(declaration, parser);
        pRoot.push_back(declaration);
    }
    return pRoot;
}

AstFuncDeclaration* ParseFunction(Parser& parser, const Token& name)
{
    AstFuncDeclaration* function = New<AstFuncDeclaration>(*parser.allocator, name.GetText());

	// Compile the parameter list.
	parser.Consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");
	function->declarationList = ParseDeclarationList(parser);
	parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
	parser.Consume(TOKEN_COLON, "Expect ':' after close parenthesis.");
    function->returnType = ParseTypeUsage(parser);

	// The body.
	parser.Consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");

	function->body = ParseFunctionBody(parser);

	parser.Consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
    return function;
}

void ClassDeclaration(AstNode*& out, Parser& parser, Token& name)
{
    AstStructDeclaration* structObj = New<AstStructDeclaration>(*parser.allocator, name.GetText());
    out = structObj;
	// // uint8_t nameConstant = parser.IdentifierConstant(name);
	// // DeclareVariable(parser, name);

	// // parser.EmitBytes(OP_CLASS, nameConstant);
	// // DefineVariable(parser, nameConstant);

	// // ClassCompiler classCompiler(name);
	// // classCompiler.enclosing = parser.currentClass;
	// // classCompiler.hasSuperclass = false;
	// // parser.currentClass = &classCompiler;

	// if (parser.Match(TOKEN_LESS))
	// {
	// 	// parser.Consume(TOKEN_IDENTIFIER, "Expect superclass name.");
	// 	// Variable(nullNode, parser, false);
	// 	// Token superName = parser.PreviousToken();
	// 	// if (IdentifiersEqual(name, superName)) {
	// 	// 	parser.Error("A class cannot inherit from itself.");
	// 	// }
	// 	// parser.BeginScope();
	// 	// AddLocal(parser, Token("super", 5));
	// 	// DefineVariable(parser, 0);
	// 	// NamedVariable(parser, name, false);
	// 	// parser.EmitByte(OP_INHERIT);
	// 	// classCompiler.hasSuperclass = true;
	// }

	// parser.Consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");
    // parser.Consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");

	// structObj->body = ParseFunctionBody(parser);

	// parser.Consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
    Statement(structObj->body, parser);

    // Parse The Class Body
    // for (Token curr = parser.CurrentToken(); ; curr = parser.CurrentToken())
    // {
    //     if (parser.GetLexer().TokenMatch(curr, "}") || parser.GetLexer().TokenMatch(curr, ")"))
    //         break;
    //     AstNode* declaration = NULL;
    //     Statement(declaration, parser);
    //     if (declaration)
    //         structObj->declarationList.push_back(declaration);
    // }

	// parser.Consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");
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