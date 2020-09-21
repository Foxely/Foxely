/*
** EPITECH PROJECT, 2019
** CodeGenerator
** File description:
** Created by besseausamuel
*/

#include "ParserHelper.h"
#include <cstring>

bool ParserHelper::Init(const std::string& str)
{
    if (!oLexer.Process(str))
        return false;

    oLexer.Begin();
    NextToken();

    return true;
}

void ParserHelper::Define(const std::string& strId, const std::string& strValue, bool bAddInTrash)
{
    oLexer.Define(strId, strValue, bAddInTrash);
}

Lexer& ParserHelper::GetLexer()
{
    return oLexer;
}

const Lexer& ParserHelper::GetLexer() const
{
    return oLexer;
}

void ParserHelper::NextToken()
{
    oCurrentToken = oLexer.NextToken();
}

const Token& ParserHelper::PreviousToken() const
{
    return oPreviousToken;
}

const Token& ParserHelper::CurrentToken() const
{
    return oCurrentToken;
}

void ParserHelper::AdvanceToken(const token_advance_mode mode)
{
	std::string errorType = std::string("ERROR");
    if (CurrentToken().m_oType == errorType)
    {
        ErrorAtCurrent(CurrentToken().GetText().c_str());
        return;
    }

    if (e_advance == mode)
    {
        NextToken();
    }
}

bool ParserHelper::IsTokenType(const std::string& strType, bool bAdvance)
{
    if (oCurrentToken.m_oType == strType && bAdvance) {
        NextToken();
        return (true);
    }
    return (false);
}

bool ParserHelper::IsToken(int iType, bool bAdvance)
{
    if (oCurrentToken.m_oType == iType && bAdvance) {
        NextToken();
        return (true);
    }
    return (false);
}

bool ParserHelper::IsToken(const std::string& strValue, bool bAdvance)
{
    if (oLexer.TokenMatch(oCurrentToken, strValue) && bAdvance)
    {
        NextToken();
        return true;
    }
    return false;
}


bool ParserHelper::IsToken(const std::string& strType, const std::string& strValue, bool bAdvance)
{
    if (oCurrentToken.m_oType == strType && oLexer.TokenMatch(oCurrentToken, strValue) && bAdvance)
    {
        NextToken();
        return true;
    }
    return false;
}

bool ParserHelper::IsToken(const std::string& strType, const char* strValue, bool bAdvance)
{
    if (oCurrentToken.m_oType == strType && oLexer.TokenMatch(oCurrentToken, strValue) && bAdvance)
    {
        NextToken();
        return true;
    }
    return false;
}

bool ParserHelper::IsToken(int iType, const std::string& strValue, bool bAdvance)
{
    if (oCurrentToken.m_oType == iType && oLexer.TokenMatch(oCurrentToken, strValue) && bAdvance)
    {
        NextToken();
        return true;
    }
    return false;
}

bool ParserHelper::IsToken(int iType, const char* strValue, bool bAdvance)
{
    if (oCurrentToken.m_oType == iType && oLexer.TokenMatch(oCurrentToken, strValue) && bAdvance)
    {
        NextToken();
        return true;
    }
    return false;
}

bool ParserHelper::IsTokenThenAssign(const std::string& strType, Token& oToken, bool bAdvance)
{
    if (oCurrentToken.m_oType == strType && bAdvance)
    {
        oToken = oCurrentToken;
        NextToken();
        return true;
    }
    return false;
}

bool ParserHelper::IsTokenThenAssign(const std::string& strType, const char* strValue, Token& oToken, bool bAdvance)
{
    if (oCurrentToken.m_oType == strType && oLexer.TokenMatch(oCurrentToken, strValue) && bAdvance)
    {
        oToken = oCurrentToken;
        NextToken();
        return true;
    }
    return false;
}

template <typename Allocator, template <typename, typename> class Container>
bool ParserHelper::IsTokenThenAssign(const std::string& strType, Container<Token&, Allocator>& oTokenList, bool bAdvance)
{
    if (oCurrentToken.m_oType == strType && bAdvance)
    {
        oTokenList.push_back(oCurrentToken.GetText());
        NextToken();
        return true;
    }
    return false;
}

template <typename Allocator, template <typename, typename> class Container>
bool ParserHelper::IsTokenThenAssign(const std::string& strType, const char* strValue, Container<Token&, Allocator>& oTokenList, bool bAdvance)
{
    if (oCurrentToken.m_oType == strType && oLexer.TokenMatch(oCurrentToken, strValue) && bAdvance)
    {
        oTokenList.push_back(oCurrentToken.GetText());
        NextToken();
        return true;
    }
    return false;
}


bool ParserHelper::IsTokenThenAssign(int iType, Token& oToken, bool bAdvance)
{
    if (oCurrentToken.m_oType == iType && bAdvance)
    {
        oToken = oCurrentToken;
        NextToken();
        return true;
    }
    return false;
}

bool ParserHelper::IsTokenThenAssign(int iType, const char* strValue, Token& oToken, bool bAdvance)
{
    if (oCurrentToken.m_oType == iType && oLexer.TokenMatch(oCurrentToken, strValue) && bAdvance)
    {
        oToken = oCurrentToken;
        NextToken();
        return true;
    }
    return false;
}

template <typename Allocator, template <typename, typename> class Container>
bool ParserHelper::IsTokenThenAssign(int iType, Container<Token&, Allocator>& oTokenList, bool bAdvance)
{
    if (oCurrentToken.m_oType == iType && bAdvance)
    {
        oTokenList.push_back(oCurrentToken.GetText());
        NextToken();
        return true;
    }
    return false;
}

template <typename Allocator, template <typename, typename> class Container>
bool ParserHelper::IsTokenThenAssign(int iType, const char* strValue, Container<Token&, Allocator>& oTokenList, bool bAdvance)
{
    if (oCurrentToken.m_oType == iType && oLexer.TokenMatch(oCurrentToken, strValue) && bAdvance)
    {
        oTokenList.push_back(oCurrentToken.GetText());
        NextToken();
        return true;
    }
    return false;
}

bool ParserHelper::PeekTokenIsType(const std::string& strType)
{
    return (oCurrentToken.m_oType == strType);
}

bool ParserHelper::PeekTokenIsType(int iType)
{
    return (oCurrentToken.m_oType == iType);
}

bool ParserHelper::PeekTokenIs(const std::string& s)
{
    return (oLexer.TokenMatch(CurrentToken(), s));
}

void ParserHelper::ErrorAtCurrent(const char* message)
{
    ErrorAt(CurrentToken(), message);
}

void ParserHelper::Error(const char* message)
{
    ErrorAt(PreviousToken(), message);
}

void ParserHelper::ErrorAt(const Token& token, const char* message)
{
    if (panicMode)
        return;
    panicMode = true;
    fprintf(stderr, "[line %d] Error", token.m_iLinesTraversed);

	std::string errorType = std::string("ERROR");
    if (token.m_oType == errorType) {
        // Nothing.
    } else {
        fprintf(stderr, " at '%s'", token.m_strText.c_str());
    }

    fprintf(stderr, ": %s\n", message);
    hadError = true;
}