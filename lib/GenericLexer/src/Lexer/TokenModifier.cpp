//
// Created by besseausamuel on 04/09/2020.
//

#include "TokenModifier.h"

std::size_t TokenModifier::Process(Lexer& oLexer)
{
    std::size_t lChanges = 0;

    for (std::size_t i = 0; i < oLexer.oTokenList.size(); ++i)
    {
        if (Modify(oLexer.oTokenList[i]))
            lChanges++;
    }

    return lChanges;
}