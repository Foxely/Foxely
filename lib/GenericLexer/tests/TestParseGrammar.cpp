// /*
// ** EPITECH PROJECT, 2019
// ** directory_test
// ** File description:
// ** Created by besseausamuel
// */

// #include "gtest/gtest.h"
// #include "Lexer.h"

// // The fixture for testing class Foo.
// class ParseGrammarTest : public ::testing::Test {
// protected:
//     // You can remove any or all of the following functions if their bodies would
//     // be empty.

//     ParseGrammarTest() {
//         // You can do set-up work for each test here.
//     }

//     ~ParseGrammarTest() override {
//         // You can do clean-up work that doesn't throw exceptions here.
//     }

//     // If the constructor and destructor are not enough for setting up
//     // and cleaning up each test, you can define the following methods:

//     void SetUp() override {
//         // Code here will be called immediately after the constructor (right
//         // before each test).
//     }

//     void TearDown() override {
//         // Code here will be called immediately after each test (right
//         // before the destructor).
//     }

//     // Class members declared here can be used by all tests in the test suite
//     // for Foo.
// };


// //TEST_F(ParseGrammarTest, shouldReturnTrueIfGrammarRulesAreLoaded)
// //{
// //    Lexer oLexer;
// //    bool expected = false;
// //
// //    EXPECT_EQ(oLexer.LoadGrammar(""), expected);
// //}
// //
// //// Expression <= Number '+' Number
// //TEST_F(ParseGrammarTest, shouldReturnTLetterTerminalIfFoundWhitespaceNameTerminal)
// //{
// //    Lexer oLexer;
// //    bool expected = true;
// //    std::string code = "<Whitespace> : [t];";
// //    bool result = oLexer.LoadGrammar(code);
// //    helper::Dump(oLexer);
// //
// //    EXPECT_EQ(result, expected);
// //    EXPECT_EQ(oLexer.m_oTerminalNames.size(), 1);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Whitespace"].m_eType, Definition::TerminalType::Terminal);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Whitespace"].m_strValue, "t");
// //}
// //
// //TEST_F(ParseGrammarTest, shouldReturnSpaceCharacterIfFoundWhitespaceNameTerminal)
// //{
// //    Lexer oLexer;
// //    bool expected = true;
// //    std::string code = "<Whitespace> : [ ];";
// //    bool result = oLexer.LoadGrammar(code);
// //
// //    EXPECT_EQ(result, expected);
// //    EXPECT_EQ(oLexer.m_oTerminalNames.size(), 1);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Whitespace"].m_eType, Definition::TerminalType::Terminal);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Whitespace"].m_strValue, " ");
// //}
// //
// //TEST_F(ParseGrammarTest, shouldReturnAllCharacterInTheAreaIfFoundWhitespaceNameTerminal)
// //{
// //    Lexer oLexer;
// //    bool expected = true;
// //    std::string code = "<Whitespace> : [ \t\n\r];";
// //    bool result = oLexer.LoadGrammar(code);
// //
// //    EXPECT_EQ(result, expected);
// //    EXPECT_EQ(oLexer.m_oTerminalNames.size(), 1);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Whitespace"].m_eType, Definition::TerminalType::Terminal);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Whitespace"].m_strValue, " \t\n\r");
// //}
// //
// //TEST_F(ParseGrammarTest, shouldContainsAllCharactersInWhitespaceListIfFoundWhitespaceNameTerminal)
// //{
// //    Lexer oLexer;
// //    bool expected = true;
// //    std::string code = "<Whitespace> : [ \t\n\r];";
// //    bool result = oLexer.LoadGrammar(code);
// //
// //    EXPECT_EQ(result, expected);
// //    EXPECT_EQ(oLexer.m_oTerminalNames.size(), 1);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Whitespace"].m_eType, Definition::TerminalType::Terminal);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Whitespace"].m_strValue, " \t\n\r");
// //    ASSERT_EQ(oLexer.m_oWhitespaces.size(), 4);
// //    EXPECT_EQ(oLexer.m_oWhitespaces[0], ' ');
// //    EXPECT_EQ(oLexer.m_oWhitespaces[1], '\t');
// //    EXPECT_EQ(oLexer.m_oWhitespaces[2], '\n');
// //    EXPECT_EQ(oLexer.m_oWhitespaces[3], '\r');
// //}
// //
// //TEST_F(ParseGrammarTest, shouldReturnAllCharactersInLowLetterAreaIfFoundWordTerminalContainLowLetterArea)
// //{
// //    Lexer oLexer;
// //    bool expected = true;
// //    std::string code = "<Word>: [a-z];";
// //    bool result = oLexer.LoadGrammar(code);
// //
// //    EXPECT_EQ(result, expected);
// //    EXPECT_EQ(oLexer.m_oTerminalNames.size(), 1);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Word"].m_eType, Definition::TerminalType::Terminal);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Word"].m_strValue, "a-z");
// //}
// //
// //TEST_F(ParseGrammarTest, shouldReturnRangeCharactersOfLowLetterAreaIfFoundWordTerminalContainLowLetterArea)
// //{
// //    Lexer oLexer;
// //    bool expected = true;
// //    std::string code = "<Word>: [a-d];";
// //    bool result = oLexer.LoadGrammar(code);
// //
// //    EXPECT_EQ(result, expected);
// //    EXPECT_EQ(oLexer.m_oTerminalNames.size(), 1);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Word"].m_eType, Definition::TerminalType::Terminal);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Word"].m_strValue, "a-d");
// //    ASSERT_EQ(oLexer.m_oIdentifierCharacters.size(), 4);
// //    EXPECT_EQ(oLexer.m_oIdentifierCharacters[0], 'a');
// //    EXPECT_EQ(oLexer.m_oIdentifierCharacters[1], 'b');
// //    EXPECT_EQ(oLexer.m_oIdentifierCharacters[2], 'c');
// //    EXPECT_EQ(oLexer.m_oIdentifierCharacters[3], 'd');
// //}
// //
// //TEST_F(ParseGrammarTest, shouldReturnCommaSymbolIfFoundSingleQuote)
// //{
// //    Lexer oLexer;
// //    bool expected = true;
// //    std::string code = "<Comma>: ',';";
// //    bool result = oLexer.LoadGrammar(code);
// //
// //    EXPECT_EQ(result, expected);
// //    EXPECT_EQ(oLexer.m_oTerminalNames.size(), 1);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Comma"].m_eType, Definition::TerminalType::Symbol);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Comma"].m_strValue, ",");
// //}
// //
// //TEST_F(ParseGrammarTest, shouldContainCommaInSymbolListIfFoundSingleQuote)
// //{
// //    Lexer oLexer;
// //    bool expected = true;
// //    std::string code = "<Comma>: ',';";
// //    bool result = oLexer.LoadGrammar(code);
// //
// //    EXPECT_EQ(result, expected);
// //    EXPECT_EQ(oLexer.m_oTerminalNames.size(), 1);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Comma"].m_eType, Definition::TerminalType::Symbol);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Comma"].m_strValue, ",");
// //    ASSERT_EQ(oLexer.m_oSymbols.size(), 1);
// //    EXPECT_EQ(oLexer.m_oSymbols[0], ",");
// //}
// //
// //TEST_F(ParseGrammarTest, shouldReturnRangeCharactersOfNumberInNumberListIfFoundNumberTerminal)
// //{
// //    Lexer oLexer;
// //    bool expected = true;
// //    std::string code = "<Number>: [0-3];";
// //    bool result = oLexer.LoadGrammar(code);
// //
// //    EXPECT_EQ(result, expected);
// //    EXPECT_EQ(oLexer.m_oTerminalNames.size(), 1);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Number"].m_eType, Definition::TerminalType::Terminal);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Number"].m_strValue, "0-3");
// //    ASSERT_EQ(oLexer.m_oNumbers.size(), 4);
// //    EXPECT_EQ(oLexer.m_oNumbers[0], '0');
// //    EXPECT_EQ(oLexer.m_oNumbers[1], '1');
// //    EXPECT_EQ(oLexer.m_oNumbers[2], '2');
// //    EXPECT_EQ(oLexer.m_oNumbers[3], '3');
// //}
// //
// //TEST_F(ParseGrammarTest, shouldReturnTheExpressionArrayIfFoundExpressionTerminal)
// //{
// //    Lexer oLexer;
// //    bool expected = true;
// //    std::string code = "<Number>: [0-3]; <Expression>: <Number> '+' <Number>;";
// //    bool result = oLexer.LoadGrammar(code);
// //    std::map<std::string, std::string> d;
// //
// //    EXPECT_EQ(result, expected);
// //    ASSERT_EQ(oLexer.m_oTerminalNames.size(), 1);
// //    ASSERT_EQ(oLexer.m_oNonTerminalNames.size(), 1);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Number"].m_eType, Definition::TerminalType::Terminal);
// //    EXPECT_EQ(oLexer.m_oTerminalNames["Number"].m_strValue, "0-3");
// //    ASSERT_EQ(oLexer.m_oNumbers.size(), 4);
// //    EXPECT_EQ(oLexer.m_oNumbers[0], '0');
// //    EXPECT_EQ(oLexer.m_oNumbers[1], '1');
// //    EXPECT_EQ(oLexer.m_oNumbers[2], '2');
// //    EXPECT_EQ(oLexer.m_oNumbers[3], '3');
// //    ASSERT_EQ(oLexer.m_oNonTerminalNames["Expression"].size(), 3);
// //    EXPECT_EQ(oLexer.m_oNonTerminalNames["Expression"][0].m_strValue, "Number");
// //    EXPECT_EQ(oLexer.m_oNonTerminalNames["Expression"][0].m_eType, Definition::TerminalType::TerminalRef);
// //    EXPECT_EQ(oLexer.m_oNonTerminalNames["Expression"][1].m_strValue, "+");
// //    EXPECT_EQ(oLexer.m_oNonTerminalNames["Expression"][1].m_eType, Definition::TerminalType::Symbol);
// //    EXPECT_EQ(oLexer.m_oNonTerminalNames["Expression"][2].m_strValue, "Number");
// //    EXPECT_EQ(oLexer.m_oNonTerminalNames["Expression"][2].m_eType, Definition::TerminalType::TerminalRef);
// //}