// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "basic_compiler/cli/TakeOptValues.h"

using namespace gwbasic::cli;

/***
 * Test: CLI_TakeOptValue.MultiName_FirstNameMatches
 * Purpose: Validate multi-name option parsing recognizes the first alias and consumes its value.
 * Components Under Test: cli::takeOptValue(const char*, std::initializer_list<const char*>, ...).
 * Expected Behavior: Returns true, advances index, and captures the value.
 */
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
