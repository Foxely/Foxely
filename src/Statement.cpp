
#include "Parser.h"
#include "compiler.h"
#include "object.hpp"
#include "vm.hpp"
#include "gc.hpp"
#include "statement.hpp"

void ReturnStatement(Parser& parser)
{
    if (parser.currentCompiler->type == TYPE_SCRIPT)
        parser.Error("Cannot return from top-level code.");
    if (parser.Match(TOKEN_SEMICOLON)) {
        parser.EmitReturn();
    } else {
        if (parser.currentCompiler->type == TYPE_INITIALIZER) {
            parser.Error("Cannot return a value from an initializer.");
        }
        Expression(parser);
        parser.Consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
        parser.EmitByte(OP_RETURN);
    }
}

void IfStatement(Parser& parser)
{
    parser.Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    Expression(parser);
    parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int then_jump = parser.EmitJump(OP_JUMP_IF_FALSE);
    parser.EmitByte(OP_POP);
    Statement(parser);

    int else_jump = parser.EmitJump(OP_JUMP);
    parser.PatchJump(then_jump);
    parser.EmitByte(OP_POP);

    if (parser.Match(TOKEN_ELSE))
        Statement(parser);

    parser.PatchJump(else_jump);
}

void Statement(Parser& parser)
{
	if (parser.Match(TOKEN_PRINT))
	{
		PrintStatement(parser);
	}
	else if (parser.Match(TOKEN_FOR))
	{
		ForStatement(parser);
	}
	else if (parser.Match(TOKEN_IF))
	{
		IfStatement(parser);
	}
	else if (parser.Match(TOKEN_RETURN))
	{
		ReturnStatement(parser);
	}
	// else if (parser.Match(TOKEN_IMPORT))
	// {

	// }
	else if (parser.Match(TOKEN_WHILE))
	{
		WhileStatement(parser);
	}
	else if (parser.Match(TOKEN_LEFT_BRACE))
	{
		parser.BeginScope();
		Block(parser);
		parser.EndScope();
	}
	else
	{
		ExpressionStatement(parser);
	}
}

void PrintStatement(Parser& parser)
{
	Expression(parser);
	parser.Consume(TOKEN_SEMICOLON, "Expect ';' after value.");
	parser.EmitByte(OP_PRINT);
}

void ExpressionStatement(Parser& parser)
{
    if  (parser.PeekTokenIsType(TOKEN_IDENTIFIER) &&
        (parser.PeekNextTokenIsType(TOKEN_COLON) || parser.PeekNextTokenIsType(TOKEN_DOUBLE_COLON)))
    {
	    Expression(parser);
    }
    else
    {
	    Expression(parser);
	    parser.Consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
        parser.EmitByte(OP_POP);
    }
}

void WhileStatement(Parser& parser)
{
    int loop_start = parser.compilingChunk->m_iCount;
    parser.Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    Expression(parser);
    parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int exit_jump = parser.EmitJump(OP_JUMP_IF_FALSE);
    parser.EmitByte(OP_POP);
    Statement(parser);
    parser.EmitLoop(loop_start);
    parser.PatchJump(exit_jump);
    parser.EmitByte(OP_POP);
}

void for_loop(Parser& parser, int *exit_jump)
{
    Expression(parser);
    parser.Consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

    *exit_jump = parser.EmitJump(OP_JUMP_IF_FALSE);
    parser.EmitByte(OP_POP);
}

void for_increment(Parser& parser, int *loop_start)
{
    int body_jump = parser.EmitJump(OP_JUMP);
    int increment_start = parser.compilingChunk->m_iCount;

    Expression(parser);
    parser.EmitByte(OP_POP);
    parser.Consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");
    parser.EmitLoop(*loop_start);
    *loop_start = increment_start;
    parser.PatchJump(body_jump);
}

void ForStatement(Parser& parser)
{
    parser.BeginScope();
    // parser.BeginScope(parser->currentCompiler);
    parser.Consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
    if (parser.Match(TOKEN_SEMICOLON)) {
    } else if (parser.Match(TOKEN_VAR)) {
        Token name = parser.PreviousToken();
		if (parser.IsToken(TOKEN_COLON) && parser.IsToken(TOKEN_VAR)) {
			VarDeclaration(parser, name);
		}
    } else {
        ExpressionStatement(parser);
    }
    int loop_start = parser.compilingChunk->m_iCount;
	int exit_jump = -1;

    if (!parser.Match(TOKEN_SEMICOLON))
        for_loop(parser, &exit_jump);
    if (!parser.Match(TOKEN_RIGHT_PAREN))
        for_increment(parser, &loop_start);
    Statement(parser);
    parser.EmitLoop(loop_start);
    if (exit_jump != -1) {
        parser.PatchJump(exit_jump);
        parser.EmitByte(OP_POP);
    }
    parser.EndScope();
    // parser.EndScope(parser->currentCompiler);
}

// void import_statement(parser_t *parser, scanner_t *scanner)
// {
//     consume(TOKEN_STRING, "Expect string after import.");

//     int import_constant = make_constant(parser, OBJ_VAL(copy_string(
//             parser->vm,
//             parser->previous.start + 1, parser->previous.length - 2)));

//     consume(TOKEN_SEMICOLON, "Expect ';' after import.");

//     parser.EmitBytes(parser, OP_IMPORT, import_constant);
// }
