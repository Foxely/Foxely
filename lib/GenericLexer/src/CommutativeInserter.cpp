//
// Created by besseausamuel on 04/09/2020.
//

#include "CommutativeInserter.h"
#include "TokenInserter.h"
#include "Lexer.h"

CommutativeInserter::CommutativeInserter() : TokenInserter(2) {}

void CommutativeInserter::IgnoreSymbol(const std::string& strSymbol)
{
    oIgnoreSet.insert(strSymbol);
}

int CommutativeInserter::Insert(const Token& oT0, const Token& oT1, Token& oNewToken)
{
    oNewToken.m_oType     = "Symbol";
    oNewToken.m_strText    = "*";
    oNewToken.m_iLength = oT1.m_iLength;
    bool match         = false;

    if (oT0.m_oType == "Symbol")
    {
        if (oIgnoreSet.find(oT0.m_strText) != oIgnoreSet.end())
        {
            return -1;
        }
        else if (!oT0.m_strText.empty() && oT0.m_strText[0] == '$')
        {
            return -1;
        }
    }

    if (oT1.m_oType == "Symbol")
    {
        if (oIgnoreSet.find(oT1.m_strText) != oIgnoreSet.end())
        {
            return -1;
        }
    }

    if ((oT0.m_oType == "Number"     ) && (oT1.m_oType == "Identifier"     )) match = true;
    else if ((oT0.m_oType == "Number"    ) && (helper::TokenIs(oT0, "Symbol", "("))) match = true;
    else if ((oT0.m_oType == "Number"     ) && (helper::TokenIs(oT0, "Symbol", "{"))) match = true;
    else if ((oT0.m_oType == "Number"     ) && (helper::TokenIs(oT0, "Symbol", "["))) match = true;
    else if ((oT0.m_oType == "Identifier"     ) && (oT1.m_oType == "Number"     )) match = true;
    else if ((helper::TokenIs(oT0, "Symbol", ")")) && (oT1.m_oType == "Number"     )) match = true;
    else if ((helper::TokenIs(oT0, "Symbol", "}")) && (oT1.m_oType == "Number"     )) match = true;
    else if ((helper::TokenIs(oT0, "Symbol", "]")) && (oT1.m_oType == "Number"     )) match = true;
    else if ((helper::TokenIs(oT0, "Symbol", ")")) && (oT1.m_oType == "Identifier"     )) match = true;
    else if ((helper::TokenIs(oT0, "Symbol", "}")) && (oT1.m_oType == "Identifier"     )) match = true;
    else if ((helper::TokenIs(oT0, "Symbol", "]")) && (oT1.m_oType == "Identifier"     )) match = true;

    return (match) ? 1 : -1;
}