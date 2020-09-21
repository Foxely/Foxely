//
// Created by besseausamuel on 04/09/2020.
//

#ifndef DIRECTORY_TEST_TOKENSCANNER_H
#define DIRECTORY_TEST_TOKENSCANNER_H

#include <iostream>
#include "Lexer.h"

class TokenScanner : public HelperInterface
{
public:

    virtual ~TokenScanner() {}

    TokenScanner(const std::size_t& stride);

    std::size_t Process(Lexer& oLexer);

    virtual bool operator()(const Token&)
    {
        return false;
    }

    virtual bool operator()(const Token&, const Token&)
    {
        return false;
    }

    virtual bool operator()(const Token&, const Token&, const Token&)
    {
        return false;
    }

    virtual bool operator()(const Token&, const Token&, const Token&, const Token&)
    {
        return false;
    }

private:

    std::size_t m_iStride;
};

#endif //DIRECTORY_TEST_TOKENSCANNER_H
