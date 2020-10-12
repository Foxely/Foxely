
#include "Parser.h"
#include "statement.hpp"
#include "ast.hpp"
#include "LinearAllocator.h"

void ReturnStatement(AstNode*& out, Parser& parser)
{
    // if (parser.currentCompiler->type == TYPE_SCRIPT) {
    //     parser.Error("Cannot return from top-level code.");
    // }
    if (parser.Match(TOKEN_SEMICOLON)) {
        out = New<Return>(*parser.allocator);
    } else {
        // if (parser.currentCompiler->type == TYPE_INITIALIZER) {
        //     parser.Error("Cannot return a value from an initializer.");
        // }
        AstNode* expression = NULL;
        Expression(expression, parser);
        out = New<Return>(*parser.allocator, expression);
        parser.Consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
    }
}

void IfStatement(AstNode*& out, Parser& parser)
{
    AstIf* ifStatement = New<AstIf>(*parser.allocator);
    parser.Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    Expression(ifStatement->condition, parser);
    parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    // int then_jump = parser.EmitJump(OP_JUMP_IF_FALSE);
    // parser.EmitByte(OP_POP);
    Statement(ifStatement->body, parser);

    // int else_jump = parser.EmitJump(OP_JUMP);
    // parser.PatchJump(then_jump);
    // parser.EmitByte(OP_POP);

    if (parser.Match(TOKEN_ELSE))
        Statement(ifStatement->elseBody, parser);
    
    out = ifStatement;
    // parser.PatchJump(else_jump);
}

void Statement(AstNode*& out, Parser& parser)
{
	if (parser.Match(TOKEN_PRINT))
	{
		PrintStatement(out, parser);
	}
	else if (parser.Match(TOKEN_FOR))
	{
		ForStatement(out, parser);
	}
	else if (parser.Match(TOKEN_IF))
	{
		IfStatement(out, parser);
	}
	else if (parser.Match(TOKEN_RETURN))
	{
		ReturnStatement(out, parser);
	}
	else if (parser.Match(TOKEN_IMPORT))
	{
        ImportStatement(out, parser);
	}
	else if (parser.Match(TOKEN_WHILE))
	{
		WhileStatement(out, parser);
	}
	else if (parser.Match(TOKEN_LEFT_BRACE))
	{
		//parser.BeginScope();
		Block(out, parser);
		//parser.EndScope();
	}
	else
	{
		ExpressionStatement(out, parser);
	}
}

void ImportStatement(AstNode*& out, Parser& parser)
{
    parser.Consume(TOKEN_STRING, "Expect string after import.");
    Type* string = New<Type>(*parser.allocator, parser.PreviousToken().GetText());

    parser.Consume(TOKEN_SEMICOLON, "Expect ';' after import.");
    out = New<Import>(*parser.allocator, string);
}

void PrintStatement(AstNode*& out, Parser& parser)
{
	parser.Consume(TOKEN_STRING, "Expect string type for the print function.");
    Type* string = New<Type>(*parser.allocator, parser.PreviousToken().GetText());
    Print* print = New<Print>(*parser.allocator, string);

    while (parser.Match(TOKEN_COMMA))
    {
        AstNode* arg = nullptr;
        Expression(arg, parser);

        if (arg)
            print->args.push_back(arg);
    }
	parser.Consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    out = print;
    // parser.pRoot.push_back(New<Print>(string));
}

void ExpressionStatement(AstNode*& out, Parser& parser)
{
    if  (parser.PeekTokenIsType(TOKEN_IDENTIFIER) &&
        (parser.PeekNextTokenIsType(TOKEN_COLON) || parser.PeekNextTokenIsType(TOKEN_DOUBLE_COLON)))
    {
	    Expression(out, parser);
    }
    else
    {
	    Expression(out, parser);
	    parser.Consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
        // parser.EmitByte(OP_POP);
    }
}

void WhileStatement(AstNode*& out, Parser& parser)
{
    AstWhile* astWhile = New<AstWhile>(*parser.allocator);
    out = astWhile;

    parser.Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    Expression(astWhile->condition, parser);
    parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    astWhile->body = ParseFunctionBody(parser);
    // Statement(astWhile->body, parser);
}

void ForStatement(AstNode*& out, Parser& parser)
{
    AstFor* forStatement = New<AstFor>(*parser.allocator);
    out = forStatement;
    parser.Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
    if (parser.Match(TOKEN_SEMICOLON)) {
    } else if (parser.Match(TOKEN_IDENTIFIER)) {
        Variable(forStatement->initialize, parser, true);
    } else {
        ExpressionStatement(forStatement->initialize, parser);
    }

    if (!parser.Match(TOKEN_SEMICOLON)) {
        Expression(forStatement->condition, parser);
        parser.Consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");
    }
    if (!parser.Match(TOKEN_RIGHT_PAREN)) {
        Expression(forStatement->increment, parser);
        parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");
    }
	parser.Consume(TOKEN_LEFT_BRACE, "Expect '{' before for body.");

    forStatement->body = ParseFunctionBody(parser);

	parser.Consume(TOKEN_RIGHT_BRACE, "Expect '}' after for body.");
}
