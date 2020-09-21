//
// Created by besseausamuel on 04/09/2020.
//

#ifndef DIRECTORY_TEST_TOKENMODIFIER_H
#define DIRECTORY_TEST_TOKENMODIFIER_H

#include "Lexer.h"

class TokenModifier : public HelperInterface
{
public:

    std::size_t Process(Lexer& oLexer);

    virtual bool Modify(Token& t) = 0;
};

#endif //DIRECTORY_TEST_TOKENMODIFIER_H
