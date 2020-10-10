
#include "Parser.h"
#include "object.hpp"
#include "gc.hpp"
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
    int loop_start = parser.GetCurrentChunk()->m_iCount;
    parser.Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    Expression(out, parser);
    parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int exit_jump = parser.EmitJump(OP_JUMP_IF_FALSE);
    // parser.EmitByte(OP_POP);
    Statement(out, parser);
    parser.EmitLoop(loop_start);
    parser.PatchJump(exit_jump);
    // parser.EmitByte(OP_POP);
}

void for_loop(AstNode*& out, Parser& parser, int *exit_jump)
{
    Expression(out, parser);
    parser.Consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

    *exit_jump = parser.EmitJump(OP_JUMP_IF_FALSE);
    // parser.EmitByte(OP_POP);
}

void for_increment(AstNode*& out, Parser& parser, int *loop_start)
{
    int body_jump = parser.EmitJump(OP_JUMP);
    int increment_start = parser.GetCurrentChunk()->m_vCode.size();

    Expression(out, parser);
    // parser.EmitByte(OP_POP);
    parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");
    parser.EmitLoop(*loop_start);
    *loop_start = increment_start;
    parser.PatchJump(body_jump);
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
    Statement(forStatement->body, parser);
}
