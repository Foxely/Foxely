//
// Created by besseausamuel on 03/09/2020.
//

#ifndef DIRECTORY_TEST_TOKENJOINER_H
#define DIRECTORY_TEST_TOKENJOINER_H

#include "Lexer.h"

class TokenJoiner : public HelperInterface
{
public:

    std::size_t Process(Lexer& oLexer);

    virtual bool Join(const Token&, const Token&, Token&) = 0;
};

#endif //DIRECTORY_TEST_TOKENJOINER_H
