// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Symbol.h"

using namespace gwbasic;

/***
Test: SymbolHeader.ConstructAndAccessors
Purpose: Validate Symbol construction from char, C-string, and string_view, plus accessors and operators.
Components Under Test: `Symbol` constructors; `size`, `is_one`, `first`, `to_char`, `to_string_view`, `to_string`,
                       equality operators with char/string_view; stream insertion operator.
Expected Behavior: Symbols report correct size/bytes, compare equal to expected literals, and stream without nulls.
*/
TEST(SymbolHeader, ConstructAndAccessors) {
    // Single-char constructor
    Symbol s1('+');
    EXPECT_EQ(s1.size(), 1u);
    EXPECT_TRUE(s1.is_one());
    EXPECT_EQ(s1.to_char(), '+');
    EXPECT_EQ(s1.to_string_view(), std::string_view("+"));
    EXPECT_EQ(s1.to_string(), "+");

    // Literal constructors (1 and 2 chars)
    Symbol s2("-");
    EXPECT_EQ(s2.size(), 1u);
    EXPECT_EQ(s2.first(), '-');
    Symbol s3("<=");
    EXPECT_EQ(s3.size(), 2u);
    EXPECT_EQ(s3.to_string_view(), std::string_view("<="));

    // string_view ctor (1 and 2 bytes)
    Symbol s4(std::string_view("*"));
    EXPECT_EQ(s4.size(), 1u);
    EXPECT_EQ(static_cast<char>(s4), '*');
    Symbol s5(std::string_view(">="));
    EXPECT_EQ(s5.size(), 2u);

    // Equality operators
    EXPECT_TRUE(s1 == '+');
    EXPECT_TRUE('+' == s1);
    EXPECT_TRUE(s3 == std::string_view("<="));
    EXPECT_TRUE(std::string_view("<=") == s3);
    EXPECT_FALSE(s3 == std::string_view("<>"));

    // operator<< streams bytes without null
    std::ostringstream oss;
    oss << s3;
    EXPECT_EQ(oss.str(), "<=");
}
