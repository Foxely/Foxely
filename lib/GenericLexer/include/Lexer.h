
#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include "Token.h"
#include "Regex.h"


struct LessCompare
{
    inline bool operator()(const std::string& s1, const std::string& s2) const
    {
        const std::size_t length = std::min(s1.size(),s2.size());

        for (std::size_t i = 0; i < length;  ++i)
        {
            if (std::tolower(s1[i]) > std::tolower(s2[i]))
                return false;
            else if (std::tolower(s1[i]) < std::tolower(s2[i]))
                return true;
        }

        return s1.size() < s2.size();
    }
};

class Lexer
{
public:
    std::vector<Token> oTokenList;
    std::vector<Token>::iterator oTokenIterator;
    std::vector<Token>::iterator oStoreTokenIterator;
    Token oEofToken;
    const char* strEnd;
    std::vector<std::pair<StringID, std::string>> m_oAllDefines;
    std::vector<StringID> m_oTrashDefines;

private:
    int SkipComment(bool long_comment);

public:
    char* m_strFilename;
    const char* m_strCurrent;
    std::string m_strText;
    int m_iLines;
    int m_iError;

public:
    Lexer();
    ~Lexer();
    void Begin();
    bool IsEnd(const char* strItr);
    Token NextToken();
    bool Process(const std::string& strText);
    static bool TokenMatch(Token oToken, const char* string);
    static bool TokenMatch(Token oToken, const std::string& strString);
    void Store();
    void Restore();
    Token& PeekNextToken();
    bool Empty() const;
    std::size_t Size() const;
    void Clear();
    bool Finished() const;

    inline Token& operator[](const std::size_t& lIndex)
    {
        if (lIndex < oTokenList.size())
            return oTokenList[lIndex];
        else
            return oEofToken;
    }

    inline Token operator[](const std::size_t& lIndex) const
    {
        if (lIndex < oTokenList.size())
            return oTokenList[lIndex];
        else
            return oEofToken;
    }

    void AddArea(std::pair<char, char> cRange);
    void Define(const std::string& strId, const std::string& strValue, bool bAddInTrash = false);
	void Define(const int id, const std::string& strRegex, bool bAddInTrash = false);
    void DefineArea(std::string strId, char cStart, char cEnd);

};

class HelperInterface
{
public:
    virtual void Init()                     {              }
    virtual void Reset()                    {              }
    virtual bool Result()                   { return true; }
    virtual std::size_t Process(Lexer&) { return 0;    }
    virtual ~HelperInterface()             = default;
};

namespace helper
{

    inline void Dump(Lexer& oLexer)
    {
        for (std::size_t i = 0; i < oLexer.Size(); ++i)
        {
            Token oToken = oLexer[i];
            std::cout << "Token[" << i << "] " << oToken.m_oType << " --> '" << oToken.GetText() << "'" << std::endl;
        }
    }

    inline bool TokenIs(Token oToken, const std::string& eType, const char* strValue)
    {
        return oToken.m_oType == eType && Lexer::TokenMatch(oToken, strValue);
    }

    // class OperatorJoiner : public TokenJoiner
    // {
    // public:

    //     inline bool Join(const Token& oT0, const Token& oT1, Token& oToken)
    //     {
    //         //': =' --> ':='
    //         if ((oT0.m_eType == token::e_colon) && (oT1.type == token::e_eq))
    //         {
    //             t.type     = token::e_assign;
    //             t.value    = ":=";
    //             t.position = oT0.position;

    //             return true;
    //         }
    //         //'> =' --> '>='
    //         else if ((oT0.type == token::e_gt) && (oT1.type == token::e_eq))
    //         {
    //             t.type     = token::e_gte;
    //             t.value    = ">=";
    //             t.position = oT0.position;

    //             return true;
    //         }
    //         //'< =' --> '<='
    //         else if ((oT0.type == token::e_lt) && (oT1.type == token::e_eq))
    //         {
    //             t.type     = token::e_lte;
    //             t.value    = "<=";
    //             t.position = oT0.position;

    //             return true;
    //         }
    //         //'= =' --> '=='
    //         else if ((oT0.type == token::e_eq) && (oT1.type == token::e_eq))
    //         {
    //             t.type     = token::e_eq;
    //             t.value    = "==";
    //             t.position = oT0.position;

    //             return true;
    //         }
    //         //'! =' --> '!='
    //         else if ((static_cast<char>(oT0.type) == '!') && (oT1.type == token::e_eq))
    //         {
    //             t.type     = token::e_ne;
    //             t.value    = "!=";
    //             t.position = oT0.position;

    //             return true;
    //         }
    //         //'< >' --> '<>'
    //         else if ((oT0.type == token::e_lt) && (oT1.type == token::e_gt))
    //         {
    //             t.type     = token::e_ne;
    //             t.value    = "<>";
    //             t.position = oT0.position;

    //             return true;
    //         }
    //         else
    //             return false;
    //         }
    // };

    // class BracketChecker : public TokenScanner
    // {
    // public:

    //     BracketChecker() : TokenScanner(1), state_(true)
    //     {}

    //     bool result()
    //     {
    //         return state_ && stack_.empty();
    //     }

    //     token error_token()
    //     {
    //         return error_token_;
    //     }

    //     void reset()
    //     {
    //         //why? because msvc doesn't support swap properly.
    //         stack_ = std::stack<char>();
    //         state_ = true;
    //         error_token_.clear();
    //     }

    //     bool operator()(const Token& t)
    //     {
    //         if (
    //                 !t.value.empty()                         &&
    //                 (token::e_string != t.type)     &&
    //                 (TokenType::TokenAlphaNum != t.type)     &&
    //                 details::is_bracket(t.value[0])
    //             )
    //         {
    //             char c = t.value[0];

    //                 if (t.type == token::e_lbracket)    stack_.push(')');
    //             else if (t.type == token::e_lcrlbracket) stack_.push('}');
    //             else if (t.type == token::e_lsqrbracket) stack_.push(']');
    //             else if (details::is_right_bracket(c))
    //             {
    //                 if (stack_.empty())
    //                 {
    //                     state_ = false;
    //                     error_token_ = t;

    //                     return false;
    //                 }
    //                 else if (c != stack_.top())
    //                 {
    //                     state_ = false;
    //                     error_token_ = t;

    //                     return false;
    //                 }
    //                 else
    //                     stack_.pop();
    //             }
    //         }

    //         return true;
    //     }

    // private:

    //     bool state_;
    //     std::stack<char> stack_;
    //     token error_token_;
    // };

    // class SymbolReplacer : public TokenModifier
    // {
    // private:

    //     typedef std::map<std::string,std::pair<std::string,token::Tokenype>,details::ilesscompare> replace_map_t;

    // public:

    //     bool remove(const std::string& target_symbol)
    //     {
    //     replace_map_t::iterator itr = replace_map_.find(target_symbol);

    //     if (replace_map_.end() == itr)
    //         return false;

    //     replace_map_.erase(itr);

    //     return true;
    //     }

    //     bool add_replace(const std::string& target_symbol,
    //                     const std::string& replace_symbol,
    //                     const token::Tokenype Tokenype = TokenType::TokenAlphaNum)
    //     {
    //         replace_map_t::iterator itr = replace_map_.find(target_symbol);

    //         if (replace_map_.end() != itr)
    //         {
    //             return false;
    //         }

    //         replace_map_[target_symbol] = std::make_pair(replace_symbol,Tokenype);

    //         return true;
    //     }

    //     void clear()
    //     {
    //         replace_map_.clear();
    //     }

    // private:

    //     bool Modify(Token& t)
    //     {
    //         if (TokenType::TokenAlphaNum == t.type)
    //         {
    //             if (replace_map_.empty())
    //                 return false;

    //             replace_map_t::iterator itr = replace_map_.find(t.value);

    //             if (replace_map_.end() != itr)
    //             {
    //                 t.value = itr->second.first;
    //                 t.type  = itr->second.second;

    //                 return true;
    //             }
    //         }

    //         return false;
    //     }

    //     replace_map_t replace_map_;
    // };

    // class SequenceValidator : public TokenScanner
    // {
    // private:

    //     typedef std::pair<token::Tokenype,token::Tokenype> token_pair_t;
    //     typedef std::set<token_pair_t> set_t;

    // public:

    //     SequenceValidator() : TokenScanner(2)
    //     {
    //     add_invalid(TokenType::TokenNumber,TokenType::TokenNumber);
    //     add_invalid(token::e_string,token::e_string);
    //     add_invalid(TokenType::TokenNumber,token::e_string);
    //     add_invalid(token::e_string,TokenType::TokenNumber);
    //     add_invalid(token::e_string,token::e_colon);
    //     add_invalid(token::e_colon,token::e_string);
    //     add_invalid_seoT1(token::e_assign);
    //     add_invalid_seoT1(token::e_shr   );
    //     add_invalid_seoT1(token::e_shl   );
    //     add_invalid_seoT1(token::e_lte   );
    //     add_invalid_seoT1(token::e_ne    );
    //     add_invalid_seoT1(token::e_gte   );
    //     add_invalid_seoT1(token::e_lt    );
    //     add_invalid_seoT1(token::e_gt    );
    //     add_invalid_seoT1(token::e_eq    );
    //     add_invalid_seoT1(token::e_comma );
    //     add_invalid_seoT1(token::e_add   );
    //     add_invalid_seoT1(token::e_sub   );
    //     add_invalid_seoT1(token::e_div   );
    //     add_invalid_seoT1(token::e_mul   );
    //     add_invalid_seoT1(token::e_mod   );
    //     add_invalid_seoT1(token::e_pow   );
    //     add_invalid_seoT1(token::e_colon );
    //     }

    //     bool result()
    //     {
    //     return error_list_.empty();
    //     }

    //     bool operator()(const Token& oT0, const Token& oT1)
    //     {
    //     set_t::value_type p = std::make_pair(oT0.type,oT1.type);

    //     if (invalid_bracket_check(oT0.type,oT1.type))
    //     {
    //         error_list_.push_back(std::make_pair(oT0,oT1));
    //     }
    //     else if (invalid_comb_.find(p) != invalid_comb_.end())
    //         error_list_.push_back(std::make_pair(oT0,oT1));

    //     return true;
    //     }

    //     std::size_t error_count()
    //     {
    //     return error_list_.size();
    //     }

    //     std::pair<token,token> error(const std::size_t index)
    //     {
    //     if (index < error_list_.size())
    //     {
    //         return error_list_[index];
    //     }
    //     else
    //     {
    //         static const token error_token;
    //         return std::make_pair(error_token,error_token);
    //     }
    //     }

    //     void clear_errors()
    //     {
    //     error_list_.clear();
    //     }

    // private:

    //     void add_invalid(token::Tokenype base, token::Tokenype t)
    //     {
    //     invalid_comb_.insert(std::make_pair(base,t));
    //     }

    //     void add_invalid_seoT1(token::Tokenype t)
    //     {
    //     add_invalid(t,token::e_assign);
    //     add_invalid(t,token::e_shr   );
    //     add_invalid(t,token::e_shl   );
    //     add_invalid(t,token::e_lte   );
    //     add_invalid(t,token::e_ne    );
    //     add_invalid(t,token::e_gte   );
    //     add_invalid(t,token::e_lt    );
    //     add_invalid(t,token::e_gt    );
    //     add_invalid(t,token::e_eq    );
    //     add_invalid(t,token::e_comma );
    //     add_invalid(t,token::e_div   );
    //     add_invalid(t,token::e_mul   );
    //     add_invalid(t,token::e_mod   );
    //     add_invalid(t,token::e_pow   );
    //     add_invalid(t,token::e_colon );
    //     }

    //     bool invalid_bracket_check(token::Tokenype base, token::Tokenype t)
    //     {
    //     if (details::is_right_bracket(static_cast<char>(base)))
    //     {
    //         switch (t)
    //         {
    //             case token::e_string : return true;
    //             case token::e_assign : return true;
    //             default                       : return false;
    //         }
    //     }
    //     else if (details::is_left_bracket(static_cast<char>(base)))
    //     {
    //         if (details::is_right_bracket(static_cast<char>(t)))
    //             return false;
    //         else if (details::is_left_bracket(static_cast<char>(t)))
    //             return false;
    //         else
    //         {
    //             switch (t)
    //             {
    //                 case TokenType::TokenNumber : return false;
    //                 case TokenType::TokenAlphaNum : return false;
    //                 case token::e_string : return false;
    //                 case token::e_add    : return false;
    //                 case token::e_sub    : return false;
    //                 case token::e_colon  : return false;
    //                 default                       : return true;
    //             }
    //         }
    //     }
    //     else if (details::is_right_bracket(static_cast<char>(t)))
    //     {
    //         switch (base)
    //         {
    //             case TokenType::TokenNumber : return false;
    //             case TokenType::TokenAlphaNum : return false;
    //             case token::e_string : return false;
    //             case token::e_eof    : return false;
    //             case token::e_colon  : return false;
    //             default                       : return true;
    //         }
    //     }
    //     else if (details::is_left_bracket(static_cast<char>(t)))
    //     {
    //         switch (base)
    //         {
    //             case token::e_rbracket    : return true;
    //             case token::e_rsqrbracket : return true;
    //             case token::e_rcrlbracket : return true;
    //             default                            : return false;
    //         }
    //     }

    //     return false;
    //     }

    //     set_t invalid_comb_;
    //     std::deque<std::pair<token,token> > error_list_;

    // };

    // struct helper_assembly
    // {
    //     inline bool register_scanner(TokenScanner* scanner)
    //     {
    //     if (TokenScanner_list.end() != std::find(TokenScanner_list.begin(),
    //                                                 TokenScanner_list.end(),
    //                                                 scanner))
    //     {
    //         return false;
    //     }

    //     TokenScanner_list.push_back(scanner);

    //     return true;
    //     }

    //     inline bool register_modifier(TokenM
    // odifier* modifier)
    //     {
    //     if (TokenM
    // odifier_list.end() != std::find(TokenM
    // odifier_list.begin(),
    //                                                 TokenM
    //                                             odifier_list.end(),
    //                                                 modifier))
    //     {
    //         return false;
    //     }

    //     TokenM
    // odifier_list.push_back(modifier);

    //     return true;
    //     }

    //     inline bool register_joiner(token_joiner* joiner)
    //     {
    //     if (token_joiner_list.end() != std::find(token_joiner_list.begin(),
    //                                                 token_joiner_list.end(),
    //                                                 joiner))
    //     {
    //         return false;
    //     }

    //     token_joiner_list.push_back(joiner);

    //     return true;
    //     }

    //     inline bool register_inserter(token_inserter* inserter)
    //     {
    //     if (token_inserter_list.end() != std::find(token_inserter_list.begin(),
    //                                                 token_inserter_list.end(),
    //                                                 inserter))
    //     {
    //         return false;
    //     }

    //     token_inserter_list.push_back(inserter);

    //     return true;
    //     }

    //     inline bool run_modifiers(Lexer& g)
    //     {
    //     error_TokenM
    // odifier = reinterpret_cast<TokenM
    // odifier*>(0);

    //     for (std::size_t i = 0; i < TokenM
    // odifier_list.size(); ++i)
    //     {
    //         TokenM
    //     odifier& modifier = (*TokenM
    //     odifier_list[i]);

    //         modifier.reset();
    //         modifier.process(g);

    //         if (!modifier.result())
    //         {
    //             error_TokenM
    //         odifier = TokenM
    //         odifier_list[i];

    //             return false;
    //         }
    //     }

    //     return true;
    //     }

    //     inline bool run_joiners(Lexer& g)
    //     {
    //     error_token_joiner = reinterpret_cast<token_joiner*>(0);

    //     for (std::size_t i = 0; i < token_joiner_list.size(); ++i)
    //     {
    //         token_joiner& joiner = (*token_joiner_list[i]);

    //         joiner.reset();
    //         joiner.process(g);

    //         if (!joiner.result())
    //         {
    //             error_token_joiner = token_joiner_list[i];

    //             return false;
    //         }
    //     }

    //     return true;
    //     }

    //     inline bool run_inserters(Lexer& g)
    //     {
    //     error_token_inserter = reinterpret_cast<token_inserter*>(0);

    //     for (std::size_t i = 0; i < token_inserter_list.size(); ++i)
    //     {
    //         token_inserter& inserter = (*token_inserter_list[i]);

    //         inserter.reset();
    //         inserter.process(g);

    //         if (!inserter.result())
    //         {
    //             error_token_inserter = token_inserter_list[i];

    //             return false;
    //         }
    //     }

    //     return true;
    //     }

    //     inline bool run_scanners(Lexer& g)
    //     {
    //     error_TokenScanner = reinterpret_cast<TokenScanner*>(0);

    //     for (std::size_t i = 0; i < TokenScanner_list.size(); ++i)
    //     {
    //         TokenScanner& scanner = (*TokenScanner_list[i]);

    //         scanner.reset();
    //         scanner.process(g);

    //         if (!scanner.result())
    //         {
    //             error_TokenScanner = TokenScanner_list[i];

    //             return false;
    //         }
    //     }

    //     return true;
    //     }

    //     std::deque<TokenScanner*>  TokenScanner_list;
    //     std::deque<TokenM
    // odifier*> TokenM
    // odifier_list;
    //     std::deque<token_joiner*>   token_joiner_list;
    //     std::deque<token_inserter*> token_inserter_list;

    //     TokenScanner*  error_TokenScanner;
    //     TokenM
    // odifier* error_TokenM
    // odifier;
    //     token_joiner*   error_token_joiner;
    //     token_inserter* error_token_inserter;
    // };
}