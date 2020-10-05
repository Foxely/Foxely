
#include <iostream>
#include <utility>
#include <algorithm>

#include "Token.h"
#include "Lexer.h"

using namespace std;

Token::Token()
{
    m_iLength = 0;
    m_strText = string("");
    m_iLinesTraversed = 0;
    // m_strvText = string_view(m_strText);
}

Token::Token(const char* beg, size_t len)
{
    m_iLength = len;
    m_strText = string(beg, m_iLength);
    m_iLinesTraversed = 0;
    // m_strvText = string_view(m_strText);
}

Token::Token(const char* beg, const char* end)
{
    m_iLength = distance(beg, end);
    m_strText = string(beg, m_iLength);
    m_iLinesTraversed = 0;
    // m_strvText = string_view(m_strText);
}

Token::Token(StringID type, const std::string& beg, std::size_t len, std::size_t lLines)
{
    m_oType = type;
    m_iLength = len;
    m_strText = string(beg, 0, m_iLength);
    m_iLinesTraversed = lLines;
}

// @note This is probably slow but it's works!!
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

Token::Token(StringID type, const char *beg, const char *end, std::size_t lLines)
{
    m_oType = std::move(type);
    m_iLength = distance(beg, end);
    m_strText = ReplaceAll(string(beg, m_iLength), "\\n", "\n");
    m_iLinesTraversed = lLines;
}

Token::~Token()
{
}

std::string Token::GetText() const {
    return m_strText;
}

void Token::SetText(std::string& strText)
{
    m_strText.assign(strText);
    // m_strvText = string_view(m_strText);
}


/**
 * @brief Cette méthode renvoie une chaine de caractère du texte du token
 * @return une chaine de caractère
 */
char* Token::GetCString()
{
    return (char *) m_strText.c_str();
}
