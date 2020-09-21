// /*
// ** EPITECH PROJECT, 2019
// ** directory_test
// ** File description:
// ** Created by besseausamuel
// */

// #include "gtest/gtest.h"
// #include "Lexer.h"

// // The fixture for testing class Foo.
// class LexerTest : public ::testing::Test {
// protected:
//     // You can remove any or all of the following functions if their bodies would
//     // be empty.

//     LexerTest() {
//         // You can do set-up work for each test here.
//     }

//     ~LexerTest() override {
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

// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }





// //
// //TEST_F(LexerTest, shouldReturnFalseIfStringIsEmpty)
// //{
// //    Lexer oLexer;
// //    bool expected = false;
// //
// //    EXPECT_EQ(oLexer.Process(""), expected);
// //}
// //
// //TEST_F(LexerTest, shouldContain0TokenIfStringIsEmpty)
// //{
// //    int expected = 0;
// //    Lexer oLexer;
// //
// //    oLexer.Process("\0");
// //
// //    EXPECT_EQ(oLexer.oTokenList.size(), expected);
// //}
// //
// //TEST_F(LexerTest, shouldReturnFalseIfStringIsEof)
// //{
// //    Lexer oLexer;
// //    bool expected = false;
// //
// //    EXPECT_EQ(oLexer.Process("\0"), expected);
// //}
// //
// //TEST_F(LexerTest, shouldContain0TokenIfStringIsEof)
// //{
// //    int expected = 0;
// //    Lexer oLexer;
// //
// //    oLexer.Process("\0");
// //
// //    EXPECT_EQ(oLexer.oTokenList.size(), expected);
// //}
// //
// //TEST_F(LexerTest, shouldContain1TokenOfTypeAlphaNumIfStringIsHello)
// //{
// //    int expected = 1;
// //    TokenType expectedType = TokenType::TokenAlphaNum;
// //
// //    Lexer oLexer;
// //    oLexer.LoadGrammar("<Word>: [a-zA-Z];");
// //
// //
// //    oLexer.Process("Hello");
// //    oLexer.Begin();
// //
// //    Token oNextToken = oLexer.NextToken();
// //
// //    EXPECT_EQ(oLexer.oTokenList.size(), expected);
// //    EXPECT_EQ(oNextToken.GetText(), "Hello");
// //    EXPECT_EQ(oNextToken.GetType(), expectedType);
// //    EXPECT_EQ(oLexer.NextToken().GetText(), "");
// //}
// //
// //TEST_F(LexerTest, shouldContain1TokenOfTypeAlphaNumIfStringIsHello_World)
// //{
// //    int expected = 1;
// //    TokenType expectedType = TokenType::TokenAlphaNum;
// //
// //    Lexer oLexer;
// //    ASSERT_EQ(oLexer.LoadGrammar("<Word>: [A-z];"), true);
// //    oLexer.Process("Hello_World");
// //    oLexer.Begin();
// //
// //    Token oNextToken = oLexer.NextToken();
// //
// //    EXPECT_EQ(oLexer.oTokenList.size(), expected);
// //    EXPECT_EQ(oNextToken.GetText(), "Hello_World");
// //    EXPECT_EQ(oNextToken.GetType(), expectedType);
// //    EXPECT_EQ(oLexer.NextToken().GetText(), "");
// //}
// //
// //TEST_F(LexerTest, shouldContain2TokenOfTypeAlphaNumIfStringIsHelloWorld)
// //{
// //    int expected = 2;
// //    TokenType expectedType = TokenType::TokenAlphaNum;
// //
// //    Lexer oLexer;
// //
// //    oLexer.AddWhitespace(' ');
// //    ASSERT_EQ(oLexer.LoadGrammar("<Whitespace>:[ ];"
// //                                 "<Word>: [A-z];"), true);
// //    oLexer.Process("Hello World");
// //    oLexer.Begin();
// //
// //    Token oHelloToken = oLexer.NextToken();
// //    Token oWorldToken = oLexer.NextToken();
// //
// //    EXPECT_EQ(oLexer.oTokenList.size(), expected);
// //    EXPECT_EQ(oHelloToken.GetText(), "Hello");
// //    EXPECT_EQ(oHelloToken.GetType(), expectedType);
// //    EXPECT_EQ(oWorldToken.GetText(), "World");
// //    EXPECT_EQ(oWorldToken.GetType(), expectedType);
// //    EXPECT_EQ(oLexer.NextToken().GetText(), "");
// //}
// //
// //TEST_F(LexerTest, shouldContain2TokenOfTypeAlphaNumIfStringIsHelloWorldWithALotOfWhitespace)
// //{
// //    int expected = 2;
// //    TokenType expectedType = TokenType::TokenAlphaNum;
// //
// //    Lexer oLexer;
// //
// //    ASSERT_EQ(oLexer.LoadGrammar("<Whitespace>:[ \n\t];"
// //                                 "<Word>: [A-z];"), true);
// //    oLexer.Process("Hello\n    \t     \t     \n World");
// //    oLexer.Begin();
// //
// //    Token oHelloToken = oLexer.NextToken();
// //    Token oWorldToken = oLexer.NextToken();
// //
// //    EXPECT_EQ(oLexer.oTokenList.size(), expected);
// //    EXPECT_EQ(oHelloToken.GetText(), "Hello");
// //    EXPECT_EQ(oHelloToken.GetType(), expectedType);
// //    EXPECT_EQ(oWorldToken.GetText(), "World");
// //    EXPECT_EQ(oWorldToken.GetType(), expectedType);
// //    EXPECT_EQ(oLexer.NextToken().GetText(), "");
// //}
// //
// //TEST_F(LexerTest, shouldContain1TokenOfTypeNumberIfStringIs150)
// //{
// //    int expected = 1;
// //    TokenType expectedType = TokenType::TokenNumber;
// //
// //    Lexer oLexer;
// //    ASSERT_EQ(oLexer.LoadGrammar("<Whitespace>:[ \n\t];"
// //                               "<Number>: [0-9];"), true);
// //    oLexer.Process("150");
// //    oLexer.Begin();
// //    Token oNextToken = oLexer.NextToken();
// //
// //    EXPECT_EQ(oLexer.oTokenList.size(), expected);
// //    EXPECT_EQ(oNextToken.GetText(), "150");
// //    EXPECT_EQ(oNextToken.GetType(), expectedType);
// //    EXPECT_EQ(oLexer.NextToken().GetText(), "");
// //}
// //
// //TEST_F(LexerTest, shouldContain2TokenOfTypeNumberAndAlphaNumIfStringIsHello150)
// //{
// //    int expected = 1;
// //    TokenType eHello150Type = TokenType::TokenAlphaNum;
// //
// //    Lexer oLexer;
// //    ASSERT_EQ(oLexer.LoadGrammar("<Whitespace>:[ \n\t];"
// //                               "<Word>: [A-z0-9];"), true);
// //    oLexer.Process("Hello150");
// //    oLexer.Begin();
// //    Token oHello150Token = oLexer.NextToken();
// //
// //    EXPECT_EQ(oLexer.oTokenList.size(), expected);
// //    EXPECT_EQ(oHello150Token.GetText(), "Hello150");
// //    EXPECT_EQ(oHello150Token.GetType(), eHello150Type);
// //    EXPECT_EQ(oLexer.NextToken().GetText(), "");
// //}
// //
// //TEST_F(LexerTest, shouldContain2TokenOfTypeNumberAndAlphaNumIfStringIsHelloAnd150)
// //{
// //    int expected = 2;
// //    TokenType eHelloType = TokenType::TokenAlphaNum;
// //    TokenType e150Type = TokenType::TokenNumber;
// //
// //    Lexer oLexer;
// //    ASSERT_EQ(oLexer.LoadGrammar("<Whitespace>:[ \n\t];"
// //                                 "<Word>: [A-z];"
// //                                 "<Number>: [0-9];"), true);
// //    oLexer.AddWhitespace(' ');
// //
// //    oLexer.Process("Hello 150");
// //    oLexer.Begin();
// //    Token oHelloToken = oLexer.NextToken();
// //    Token o150Token = oLexer.NextToken();
// //
// //    EXPECT_EQ(oLexer.oTokenList.size(), expected);
// //    EXPECT_EQ(oHelloToken.GetText(), "Hello");
// //    EXPECT_EQ(oHelloToken.GetType(), eHelloType);
// //    EXPECT_EQ(o150Token.GetText(), "150");
// //    EXPECT_EQ(o150Token.GetType(), e150Type);
// //    EXPECT_EQ(oLexer.NextToken().GetText(), "");
// //}