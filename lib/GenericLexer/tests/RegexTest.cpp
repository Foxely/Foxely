// /*
// ** EPITECH PROJECT, 2019
// ** directory_test
// ** File description:
// ** Created by besseausamuel
// */


// #include "gtest/gtest.h"
// #include "Lexer.h"
// #include "Regex.h"

// // The fixture for testing class Foo.
// class RegexTest : public ::testing::Test {
// protected:
//     // You can remove any or all of the following functions if their bodies would
//     // be empty.

//     RegexTest() {
//         // You can do set-up work for each test here.
//     }

//     ~RegexTest() override {
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

// //int main(int argc, char **argv) {
// //    ::testing::InitGoogleTest(&argc, argv);
// //    return RUN_ALL_TESTS();
// //}


// TEST_F(RegexTest, shouldReturnFiveAIfMatchAStar)
// {
//     int   len = 0;
//     char  output[256];
//     const char* res;
//     const char error[] = "@NO";
//     const char empty[] = "";
//     const char* pattern = "a*";
//     const char* pText = "aaaaa";
//     const std::string expected = "aaaaa";

//     res = regex_search(pattern, pText, &len);
//     if (len > 0) {			memcpy(output, res, len); output[len] = '\0';	}
//     else if (len == 0)	memcpy(output, empty,strlen(empty) + 1);
//     else				memcpy(output, error,strlen(error) + 1);

//     EXPECT_EQ(output, expected);
// }

// TEST_F(RegexTest, shouldReturnNothingIfMatchAStar)
// {
//     int   len = 0;
//     char  output[256];
//     const char* res;
//     const char error[] = "@NO";
//     const char empty[] = "";
//     const char* pattern = "a*";
//     const char* pText = "";
//     const std::string expected = "";

//     res = regex_search(pattern, pText, &len);
//     if (len > 0) {			memcpy(output,res, len); output[len] = '\0';	}
//     else if (len == 0)	memcpy(output,empty,strlen(empty) + 1);
//     else				memcpy(output,error,strlen(error) + 1);

//     EXPECT_EQ(output, expected);
// }

// //TEST_F(RegexTest, shouldReturnNothingIfMatchAStar)
// //{
// //    int   len = 0;
// //    char  output[256];
// //    const char* res;
// //    const char error[] = "@NO";
// //    const char empty[] = "";
// //    const char* pattern = "a*";
// //    const char* pText = "";
// //    const std::string expected = "";
// //
// //    res = regex_search(pattern, pText, &len);
// //    if (len > 0) {			memcpy(output,res, len); output[len] = '\0';	}
// //    else if (len == 0)	memcpy(output,empty,strlen(empty) + 1);
// //    else				memcpy(output,error,strlen(error) + 1);
// //
// //    EXPECT_EQ(output, expected);
// //}