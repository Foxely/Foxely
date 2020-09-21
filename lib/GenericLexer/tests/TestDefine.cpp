///*
//** EPITECH PROJECT, 2019
//** directory_test
//** File description:
//** Created by besseausamuel
//*/
//
//#include "gtest/gtest.h"
//#include "Lexer.h"
//
//// The fixture for testing class Foo.
//class DefineTest : public ::testing::Test {
//protected:
//    // You can remove any or all of the following functions if their bodies would
//    // be empty.
//
//    DefineTest() {
//        // You can do set-up work for each test here.
//    }
//
//    ~DefineTest() override {
//        // You can do clean-up work that doesn't throw exceptions here.
//    }
//
//    // If the constructor and destructor are not enough for setting up
//    // and cleaning up each test, you can define the following methods:
//
//    void SetUp() override {
//        // Code here will be called immediately after the constructor (right
//        // before each test).
//    }
//
//    void TearDown() override {
//        // Code here will be called immediately after each test (right
//        // before the destructor).
//    }
//
//    // Class members declared here can be used by all tests in the test suite
//    // for Foo.
//};
//
////int main(int argc, char **argv) {
////    ::testing::InitGoogleTest(&argc, argv);
////    return RUN_ALL_TESTS();
////}
//
//
//TEST_F(DefineTest, shouldReturnIntegerTypeIfDefineTypeIsInteger)
//{
//    Lexer oLexer;
//    bool expected = true;
//
//    oLexer.Define("Integer","[0-9]");
//    oLexer.Define("WHITESPACE"," ");
//    bool result = oLexer.Process("19");
//
//    EXPECT_EQ(result, expected);
//    EXPECT_EQ(oLexer[0].m_strType, "Integer");
//    EXPECT_EQ(oLexer[0].GetText(), "19");
//}
//
//TEST_F(DefineTest, shouldReturn3TokensIfStringIsIntegerPlusInteger)
//{
//    Lexer oLexer;
//    bool expected = true;
//
//    oLexer.Define("Integer","[0-9]");
//    oLexer.Define("Plus","+");
//    oLexer.Define("WHITESPACE"," ");
//    bool result = oLexer.Process("19 + 25");
//
//    EXPECT_EQ(result, expected);
//    EXPECT_EQ(oLexer.oTokenList.size(), 3);
//    EXPECT_EQ(oLexer[0].m_strType, "Integer");
//    EXPECT_EQ(oLexer[0].GetText(), "19");
//    EXPECT_EQ(oLexer[1].m_strType, "Plus");
//    EXPECT_EQ(oLexer[1].GetText(), "+");
//    EXPECT_EQ(oLexer[2].m_strType, "Integer");
//    EXPECT_EQ(oLexer[2].GetText(), "25");
//}
//
//TEST_F(DefineTest, shouldReturn3TokensIfStringIsAlphaPlusInteger)
//{
//    Lexer oLexer;
//    bool expected = true;
//
//    oLexer.AddWhitespace(' ');
//    oLexer.Define("Alpha","[a-z]");
//    oLexer.Define("Integer","[0-9]");
//    oLexer.Define("Plus","+");
//    oLexer.Define("WHITESPACE"," ");
//    bool result = oLexer.Process("toto + 25");
//
//    EXPECT_EQ(result, expected);
//    EXPECT_EQ(oLexer.oTokenList.size(), 3);
//    EXPECT_EQ(oLexer[0].m_strType, "Alpha");
//    EXPECT_EQ(oLexer[0].GetText(), "toto");
//    EXPECT_EQ(oLexer[1].m_strType, "Plus");
//    EXPECT_EQ(oLexer[1].GetText(), "+");
//    EXPECT_EQ(oLexer[2].m_strType, "Integer");
//    EXPECT_EQ(oLexer[2].GetText(), "25");
//}
//
//TEST_F(DefineTest, shouldReturnAlphaNumIfStringContainAlphaNum)
//{
//    Lexer oLexer;
//    bool expected = true;
//
//    oLexer.Define("Alpha","[a-z0-9]");
//    bool result = oLexer.Process("toto25");
//
//    EXPECT_EQ(result, expected);
//    EXPECT_EQ(oLexer.oTokenList.size(), 1);
//    EXPECT_EQ(oLexer[0].m_strType, "Alpha");
//    EXPECT_EQ(oLexer[0].GetText(), "toto25");
//}
//
//TEST_F(DefineTest, shouldReturnAlphaNumAndIntegerIfStringContainIntegerFirstAndAlphaNum)
//{
//    Lexer oLexer;
//    bool expected = true;
//
//    oLexer.AddWhitespace(' ');
//    oLexer.Define("Alpha","[a-zA-Z][0-9]");
//    oLexer.Define("Integer","[0-9]");
//    bool result = oLexer.Process("4878bAdfC484");
//
//    EXPECT_EQ(result, expected);
//    EXPECT_EQ(oLexer.oTokenList.size(), 2);
//    EXPECT_EQ(oLexer[0].m_strType, "Integer");
//    EXPECT_EQ(oLexer[0].GetText(), "4878");
//    EXPECT_EQ(oLexer[1].m_strType, "Alpha");
//    EXPECT_EQ(oLexer[1].GetText(), "bAdfC484");
//}
//
//TEST_F(DefineTest, shouldReturnStringTokenIfStringContainString)
//{
//    Lexer oLexer;
//    bool expected = true;
//
//    oLexer.Define("String", "\"[ -#%-~]\"");
//    bool result = oLexer.Process("\"Salut\"");
//
//    EXPECT_EQ(result, expected);
//    EXPECT_EQ(oLexer.oTokenList.size(), 1);
//    EXPECT_EQ(oLexer[0].m_strType, "String");
//    EXPECT_EQ(oLexer[0].GetText(), "\"Salut\"");
//}