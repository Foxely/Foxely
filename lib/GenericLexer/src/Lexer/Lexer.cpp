#include <iostream>
#include <utility>
#include <vector>
#include <cstring>
#include <algorithm>
#include "Lexer.h"
#include "Regex.h"

Lexer::Lexer()
{
    m_iLines = 0;
    m_iError = 0;
    m_strFilename = nullptr;
    m_strCurrent = nullptr;
}

Lexer::~Lexer() { }

// ---------------------------------------------------------------

/**
 * @brief Cette fonction permet de récuperer le token actuel et de passer au suivant
 * @return un token avec le type correspondant au mot parsé
 */
Token Lexer::NextToken()
{
    if (oTokenList.end() != oTokenIterator)
    {
        return *oTokenIterator++;
    }
    else
        return oEofToken;
}

/**
 * @brief Cette fonction permet de vérifier que le token correspond à la string passé en paramètre
 * @param oToken Le token qui sera comparer au texte
 * @param string Le texte qui sera comparer au token
 * @return true si le token et la string correspondent et false dans le cas contraire
 */
bool Lexer::TokenMatch(Token oToken, const char* string)
{
    return (!strncmp(oToken.GetCString(), string, oToken.m_iLength) && string[oToken.m_iLength] == 0);
}

bool Lexer::TokenMatch(Token oToken, const std::string& strString)
{
    return (oToken.GetText() == strString);
}

int Lexer::SkipComment(bool long_comment)
{
    int line = 0;
    m_strCurrent += 2;
    if (long_comment) {
        while (*m_strCurrent != '*' && *(m_strCurrent + 1) != '/' && *m_strCurrent) {
            if (*m_strCurrent == '\n' || *m_strCurrent == '\0')
                ++line;
            m_strCurrent++;
        }
        m_strCurrent++;
    } else {
        while (*m_strCurrent != '\n' && *m_strCurrent) {
            m_strCurrent++;
        }
        ++line;
    }

    return (line);
}

bool Lexer::Process(const std::string& strText)
{
    m_strText    = strText;
    oTokenList.clear();

    if (strText.empty())
        return false;

    Regex re;
    bool bError = false;
    m_iLines = 1;

    while (!m_strText.empty()) {
        int iLen = 0;
        const char* pText = nullptr;
        bool bFound = false;
        std::vector<StringID>::iterator itTrash;
        int maxLen = 0;
        std::pair<StringID, std::string> defineTarget;

        for (auto& define : m_oAllDefines)
        {
            const char* pStart = nullptr;
            re.Compile(define.second.c_str());
            pStart = re.Search(m_strText.c_str(), &iLen);
            // std::cout << define.second.c_str() << std::endl;

            if (iLen > 0 && m_strText.find(pStart) == 0)
            {
                if (maxLen < iLen) {
                    defineTarget = define;
                    pText = pStart;
                    maxLen = iLen;
                }
                // std::cout << define.first << " - " << m_iLines << std::endl;
            }
        }

        if (maxLen > 0)
        {
            itTrash = std::find(m_oTrashDefines.begin(), m_oTrashDefines.end(), defineTarget.first);
            if (itTrash == m_oTrashDefines.end())
                oTokenList.emplace_back(defineTarget.first, pText, maxLen, m_iLines);
            m_strText.erase(0, maxLen);
            bFound = true;

            // if (defineTarget.first == "New Line")
            //     m_iLines++;
        }

        if (iLen == 1 && !bFound) {
            oTokenList.emplace_back(StringID(84), "Unexpected Character.", (std::size_t) 21, (std::size_t) m_iLines);
            m_strText.erase(0, 1);
        }
    }
    m_iLines++;
    return !oTokenList.empty();
}

void Lexer::Begin()
{
    oTokenIterator = oTokenList.begin();
    oStoreTokenIterator = oTokenList.begin();
}

bool Lexer::IsEnd(const char* strItr)
{
    return (strItr == strEnd);
}

void Lexer::Store()
{
    oStoreTokenIterator = oTokenIterator;
}

void Lexer::Restore()
{
    oTokenIterator = oStoreTokenIterator;
}

Token& Lexer::PeekNextToken()
{
    if (oTokenList.end() != oTokenIterator)
    {
        return *oTokenIterator;
    }
    else
        return oEofToken;
}

bool Lexer::Empty() const
{
    return oTokenList.empty();
}

std::size_t Lexer::Size() const
{
    return oTokenList.size();
}

void Lexer::Clear()
{
    m_strCurrent = 0;
    strEnd = 0;
    oTokenList.clear();
    oTokenIterator = oTokenList.end();
    oStoreTokenIterator = oTokenList.end();
}

bool Lexer::Finished() const
{
    return (oTokenList.end() == oTokenIterator);
}

void Lexer::AddArea(std::pair<char, char> cRange)
{
    // m_oAreas.emplace_back(cRange);
}

// enum class Format {
//     TEXT = 0,
//     PDF = 1000,
//     OTHER = 2000,
// };
// Format f = Format::PDF;
// int a = f;                         // error
// int b = static_cast<int>(f);

void Lexer::Define(const std::string& strId, const std::string& strRegex, bool bAddInTrash)
{
    m_oAllDefines.push_back(std::make_pair(StringID(strId), strRegex));

    if (bAddInTrash)
        m_oTrashDefines.push_back(StringID(strId));
}

void Lexer::Define(const int id, const std::string& strRegex, bool bAddInTrash)
{
    m_oAllDefines.push_back(std::make_pair(StringID(id), strRegex));

    if (bAddInTrash)
        m_oTrashDefines.push_back(StringID(id));
}

void Lexer::DefineArea(const std::string strId, char cStart, char cEnd)
{
//    // m_oAreas.insert(make_pair(strId, std::string(cStart, cEnd)));
//    if (!oDfa.StateExist(strId))
//        oDfa.AddState(DFAState(false, strId));
//    int iState = oDfa.GetStateID(strId);
//    oDfa.AddTransition(0, cStart, iState);
//    oDfa.AddTransition(iState, cStart, iState);
//    oDfa.AddTransition(iState, cEnd, iState);
//    for (int j = 32; j <= 127; j++) {
//        if (j != cStart && j != cEnd)
//            oDfa.AddTransition(iState, (char) j, iState);
//    }
}
