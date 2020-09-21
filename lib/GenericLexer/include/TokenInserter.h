//
// Created by besseausamuel on 03/09/2020.
//

#ifndef DIRECTORY_TEST_TOKENINSERTER_H
#define DIRECTORY_TEST_TOKENINSERTER_H

#include "Lexer.h"

class TokenInserter : public HelperInterface
{
protected:
    TokenInserter(const int stride);

public:
    std::size_t Process(Lexer& oLexer);

    virtual int Insert(const Token&, Token& );
    virtual int Insert(const Token&, const Token&, Token&);
    virtual inline int Insert(const Token&, const Token&, const Token&, Token&);
    virtual inline int Insert(const Token&, const Token&, const Token&, const
    Token&, Token&);
    virtual inline int Insert(const Token&, const Token&, const Token&, const
    Token&, const Token&, Token&);

private:

    std::size_t m_iStride;
};

#endif //DIRECTORY_TEST_TOKENINSERTER_H
