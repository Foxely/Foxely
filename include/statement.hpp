
#pragma once

void Statement(Parser& parser);
void PrintStatement(Parser& parser);
void ExpressionStatement(Parser& parser);
void ForStatement(Parser& parser);
void WhileStatement(Parser& parser);
void for_loop(Parser& parser, int *exit_jump);
void for_increment(Parser& parser, int *loop_start);