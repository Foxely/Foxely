//
// Created by besseausamuel on 04/09/2020.
//

#include "TokenScanner.h"

TokenScanner::TokenScanner(const std::size_t& stride) : m_iStride(stride)
{
    if (stride > 4)
    {
        throw std::invalid_argument("TokenScanner() - Invalid stride value");
    }
}

std::size_t TokenScanner::Process(Lexer& oLexer)
{
    if (!oLexer.oTokenList.empty())
    {
        for (std::size_t i = 0; i < (oLexer.oTokenList.size() - m_iStride + 1); ++i)
        {
            // Token t;
            switch (m_iStride)
            {
                case 1 :
                {
                    const Token& ooT0 = oLexer.oTokenList[i];

                    if (!operator()(ooT0)) return i;
                }
                    break;

                case 2 :
                {
                    const Token& ooT0 = oLexer.oTokenList[i    ];
                    const Token& ooT1 = oLexer.oTokenList[i + 1];

                    if (!operator()(ooT0, ooT1)) return i;
                }
                    break;

                case 3 :
                {
                    const Token& ooT0 = oLexer.oTokenList[i    ];
                    const Token& ooT1 = oLexer.oTokenList[i + 1];
                    const Token& oT2 = oLexer.oTokenList[i + 2];

                    if (!operator()(ooT0, ooT1, oT2)) return i;
                }
                    break;

                case 4 :
                {
                    const Token& ooT0 = oLexer.oTokenList[i    ];
                    const Token& ooT1 = oLexer.oTokenList[i + 1];
                    const Token& oT2 = oLexer.oTokenList[i + 2];
                    const Token& oT3 = oLexer.oTokenList[i + 3];

                    if (!operator()(ooT0, ooT1, oT2, oT3)) return i;
                }
                    break;
            }
        }
    }

    return (oLexer.oTokenList.size() - m_iStride + 1);
}