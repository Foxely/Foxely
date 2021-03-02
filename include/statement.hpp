
#ifndef FOX_STATEMENT_HPP_
#define FOX_STATEMENT_HPP_

void Statement(Parser& parser);
void PrintStatement(Parser& parser);
void ExpressionStatement(Parser& parser);
void ForStatement(Parser& parser);
void WhileStatement(Parser& parser);
void for_loop(Parser& parser, int *exit_jump);
void for_increment(Parser& parser, int *loop_start);
void ImportStatement(Parser& parser);
void SwitchStatement(Parser& parser);
void ReturnStatement(Parser& parser);
void IfStatement(Parser& parser);

#endif