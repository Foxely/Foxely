//
// Created by besseausamuel on 03/09/2020.
//

#include "TokenInserter.h"

TokenInserter::TokenInserter(const int stride) : m_iStride(stride)
{
    if (stride > 5)
    {
        throw std::invalid_argument("token_inserter() - Invalid stride value");
    }
}

std::size_t TokenInserter::Process(Lexer& oLexer)
{
    if (oLexer.oTokenList.empty())
        return 0;

    std::size_t lChanges = 0;

    for (std::size_t i = 0; i < (oLexer.oTokenList.size() - m_iStride + 1); ++i)
    {
        Token oToken;
        int insert_index = -1;

        switch (m_iStride)
        {
            case 1 : insert_index = Insert(oLexer.oTokenList[i], oToken);
                break;

            case 2 : insert_index = Insert(oLexer.oTokenList[i],oLexer.oTokenList[i + 1], oToken);
                break;

            case 3 : insert_index = Insert(oLexer.oTokenList[i],oLexer.oTokenList[i + 1],oLexer.oTokenList[i + 2], oToken);
                break;

            case 4 : insert_index = Insert(oLexer.oTokenList[i],oLexer.oTokenList[i + 1],oLexer.oTokenList[i + 2],oLexer.oTokenList[i + 3], oToken);
                break;

            case 5 : insert_index = Insert(oLexer.oTokenList[i],oLexer.oTokenList[i + 1],oLexer.oTokenList[i + 2],oLexer.oTokenList[i + 3],oLexer.oTokenList[i + 4], oToken);
                break;
        }

        if ((insert_index >= 0) && (insert_index <= (static_cast<int>(m_iStride) + 1)))
        {
            oLexer.oTokenList.insert(oLexer.oTokenList.begin() + (i + insert_index), oToken);
            lChanges++;
        }
    }

    return lChanges;
}

int TokenInserter::Insert(const Token&, Token& )
{
    return -1;
}

int TokenInserter::Insert(const Token&, const Token&, Token&)
{
    return -1;
}

int TokenInserter::Insert(const Token&, const Token&, const Token&,
    Token&)
{
    return -1;
}

int TokenInserter::Insert(const Token&, const Token&, const Token&,
    const Token&, Token&)
{
    return -1;
}

int TokenInserter::Insert(const Token&, const Token&, const Token&,
    const Token&, const Token&, Token&)
{
    return -1;
}