/*
** EPITECH PROJECT, 2019
** directory_test
** File description:
** Created by besseausamuel
*/

#pragma once
#include <vector>
#include <string>

#define Stringify(x) #x
#define LaterStringify(x) Stringify(x)

class StringID
{
public:
	int m_id;
	std::string m_strId;

	explicit StringID()
	{
		m_id = -1;
		m_strId = "";
	}

	explicit StringID(int value, const std::string& strValue) : m_strId(strValue)
	{
		m_id = value;
	}

	explicit StringID(int value) : m_strId("")
	{
		m_id = value;
	}

	explicit StringID(const std::string& value) : m_strId(value)
	{
		m_id = -1;
	}

	friend bool operator==(const StringID& id, int value)
	{
		return id.m_id == value;
	}

	friend bool operator!=(const StringID& id, int value)
	{
		return id.m_id != value;
	}

	friend bool operator!=(const StringID& id, const std::string& value)
	{
		return id.m_strId != value;
	}

	bool operator==(const std::string& value)
	{
		return m_strId == value;
	}

	friend bool operator==(const StringID& id, const std::string& value)
	{
		return id.m_strId == value;
	}

	friend bool operator==(const StringID& id, const StringID& other)
	{
		return id.m_strId == other.m_strId && id.m_id == other.m_id;
	}

	void operator=(const std::string& value)
	{
		m_strId = value;
	}

	void operator=(const char* value)
	{
		m_strId = value;
	}

	void operator=(int value)
	{
		m_id = value;
	}

	bool operator<(const StringID& y)
	{
		return m_id < y.m_id;
	}

	friend std::ostream& operator<<(std::ostream& os, const StringID& value)
	{
		if (value.m_id > -1 && !value.m_strId.empty())
		{
			os << value.m_id;
			os << " (\"";
			os << value.m_strId;
			os << "\")";
		}
		else if (value.m_id > -1)
		{
			os << value.m_id;
		}
		else if (!value.m_strId.empty())
		{
			os << "\"" << value.m_strId << "\"";
		}
		return os;
	}
};

class Token
{
public:
    std::string m_strText;
    StringID m_oType;
    int m_iLength;
    int m_iLinesTraversed;

    Token();
    Token(const char* beg, std::size_t len);
    Token(const char* beg, const char* end);
    Token(StringID type, const std::string& beg, std::size_t len, std::size_t lLines = 0);
    Token(StringID type, const char* beg, const char* end, std::size_t lLines = 0);
    ~Token();

    std::string GetText() const;
    char* GetCString();
    void SetText(std::string& strText);

    // int GetBinaryOpTypeFromToken(Lexer& oLexer);

    inline bool IsError() const
    {
        return (m_oType == 84);
        // return (m_eType == TokenType::TokenUnknown);
    }
};