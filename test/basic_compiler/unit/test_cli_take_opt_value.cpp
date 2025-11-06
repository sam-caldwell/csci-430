// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "basic_compiler/cli/TakeOptValue.h"
#include "basic_compiler/cli/TakeOptValues.h"

using namespace gwbasic::cli;

TEST(CLI_TakeOptValue, SingleName_MatchAndConsume) {
    const char* argv[] = {"prog", "--ll", "out.ll"};
    int argc = 3;
    int i = 1;
    std::optional<std::string> out;
    EXPECT_TRUE(takeOptValue(argv[i], "--ll", i, argc, const_cast<char**>(argv), out));
    EXPECT_EQ(i, 2);
    ASSERT_TRUE(out.has_value());
    EXPECT_EQ(*out, std::string("out.ll"));
}

TEST(CLI_TakeOptValue, SingleName_NoMatchOrMissingValue) {
    const char* argv[] = {"prog", "--ll"};
    int argc = 2;
    int i = 1;
    std::optional<std::string> out;
    EXPECT_FALSE(takeOptValue(argv[i], "--bc", i, argc, const_cast<char**>(argv), out));
    EXPECT_FALSE(out.has_value());
    // Missing value should not consume
    EXPECT_FALSE(takeOptValue(argv[i], "--ll", i, argc, const_cast<char**>(argv), out));
    EXPECT_EQ(i, 1);
}

TEST(CLI_TakeOptValue, MultiName_FirstNameMatches) {
    const char* argv[] = {"prog", "-ll", "x.ll"};
    int argc = 3;
    int i = 1;
    std::optional<std::string> out;
    EXPECT_TRUE(takeOptValue(argv[i], {"-ll", "--ll"}, i, argc, const_cast<char**>(argv), out));
    EXPECT_EQ(i, 2);
    ASSERT_TRUE(out.has_value());
    EXPECT_EQ(*out, std::string("x.ll"));
}

