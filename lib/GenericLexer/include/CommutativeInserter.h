//
// Created by besseausamuel on 04/09/2020.
//

#ifndef DIRECTORY_TEST_COMMUTATIVEINSERTER_H
#define DIRECTORY_TEST_COMMUTATIVEINSERTER_H

#include <set>
#include "TokenInserter.h"

class CommutativeInserter : public TokenInserter
{
public:

    CommutativeInserter();

    void IgnoreSymbol(const std::string& strSymbol);

    int Insert(const Token& oT0, const Token& oT1, Token& oNewToken);

private:

    std::set<std::string, LessCompare> oIgnoreSet;
};

#endif //DIRECTORY_TEST_COMMUTATIVEINSERTER_H
