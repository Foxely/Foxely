

#include <iostream>
#include <string>
#include "Lexer.h"

int main()
{
    Lexer oLex;
    oLex.m_bEnableError = false;
    oLex.Define("WS", "[ \t\r\b]+", true);
    oLex.Define("NewLine", "\n", true);
    oLex.Define("Number", "[0-9]+");
    oLex.Define("Identifier", "[A-Za-z_]+[0-9]*");
    oLex.Define("Eof", "\\0", true);

    std::string str;
    std::getline(std::cin, str);

    oLex.Process(str);
    helper::Dump(oLex);
}