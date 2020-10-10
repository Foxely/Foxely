
#pragma once

void Statement(AstNode*& out,Parser& parser);
void PrintStatement(AstNode*& out, Parser& parser);
void ExpressionStatement(AstNode*& out, Parser& parser);
void ForStatement(AstNode*& out, Parser& parser);
void WhileStatement(AstNode*& out, Parser& parser);
void for_loop(AstNode*& out, Parser& parser, int *exit_jump);
void for_increment(AstNode*& out, Parser& parser, int *loop_start);
void ImportStatement(AstNode*& out, Parser& parser);