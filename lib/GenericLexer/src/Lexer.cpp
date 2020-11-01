#include <iostream>
#include <utility>
#include <vector>
#include <cstring>
#include <algorithm>
#include "Lexer.h"
#include "re.h"

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

bool Lexer::Process(const std::string& strText)
{
    m_strText    = strText;
    oTokenList.clear();

    if (strText.empty())
        return false;

    m_iLines = 1;

    int iLen = 0;
    std::string pText;
    bool bFound = false;
    std::vector<StringID>::iterator itTrash;
    int maxLen = 0;
    std::pair<StringID, std::string> defineTarget;
	// helper::replaceAll(m_strText, "\\\"", "\"", m_strText.size());

    while (!m_strText.empty())
    {
        bFound = false;
        maxLen = 0;
        iLen = 0;

        for (auto& define : m_oAreaDefines)
		{
			if (define.second.m_cStart == m_strText[0])
			{
				m_strText.erase(0, 1);
				std::string newStr = "";
				int i = 0;
				for(i = 0; m_strText[i] != '"'; i++)
				{
					if (m_strText[i] == '\\')
					{
						switch(m_strText[++i])
						{
							case '\\':
								newStr += '\\';
								break;
							case 'n':
								newStr += '\n';
								break;
							case 't':
								newStr += '\t';
								break;
							case 'r':
								newStr += '\r';
								break;
							case '"':
								newStr += '"';
								break;
						}
					} else
						newStr += m_strText[i];
				}
				oTokenList.emplace_back(define.first, newStr, newStr.size(), m_iLines);
				m_strText.erase(0, i + 1);
				break;
			}
		}

        for (auto& define : m_oAllDefines)
        {
            int index = re_match(define.second.c_str(), m_strText.c_str(), &iLen);

            if (index == 0)
            {
                if (maxLen < iLen)
                {
                    defineTarget = define;
                    maxLen = iLen;
                    pText = m_strText.substr(index, maxLen);
                }
            }
        }

        if (maxLen > 0)
        {
            itTrash = std::find(m_oTrashDefines.begin(), m_oTrashDefines.end(), defineTarget.first);
			std::string newStr = "";
			int i = 0;
            if (itTrash == m_oTrashDefines.end())
			{
				for(i = 0; i < maxLen; i++)
				{
					if (m_strText[i] == '\\')
					{
						switch(m_strText[++i])
						{
							case '\\':
								newStr += '\\';
								break;
							case 'n':
								newStr += '\n';
								break;
							case 't':
								newStr += '\t';
								break;
							case 'r':
								newStr += '\r';
								break;
							case '"':
								newStr += '"';
								break;
						}
					} else
						newStr += m_strText[i];
				}
                oTokenList.emplace_back(defineTarget.first, newStr, i, m_iLines);
			}
            m_strText.erase(0, maxLen);
            bFound = true;

            if (defineTarget.first == 83)
                m_iLines++;
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

void Lexer::DefineArea(const std::string& strId, char cStart, char cEnd)
{
   m_oAreaDefines.push_back(std::make_pair(StringID(strId), Area(cStart, cEnd)));
}


void Lexer::DefineArea(const int id, char cStart, char cEnd)
{
   m_oAreaDefines.push_back(std::make_pair(StringID(id), Area(cStart, cEnd)));
}