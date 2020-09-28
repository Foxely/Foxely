//
// Created by besseausamuel on 03/09/2020.
//

#include "TokenJoiner.h"

std::size_t TokenJoiner::Process(Lexer& oLexer)
{
    if (oLexer.oTokenList.empty())
        return 0;

    std::size_t lChanges = 0;

    for (std::size_t i = 0; i < oLexer.oTokenList.size() - 1; ++i)
    {
        Token oToken;

        if (Join(oLexer.oTokenList[i],oLexer.oTokenList[i + 1], oToken))
        {
            oLexer.oTokenList[i] = oToken;
            oLexer.oTokenList.erase(oLexer.oTokenList.begin() + (i + 1));

            ++lChanges;
        }
    }

    return lChanges;
}