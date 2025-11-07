// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "basic_compiler/cli/TakeOptValue.h"

using namespace gwbasic::cli;

/***
 * Test: CLI_TakeOptValue.SingleName_MatchAndConsume
 * Purpose: Validate single-name option parsing consumes the value when present.
 * Components Under Test: cli::takeOptValue(const char*, const char*, ...).
 * Expected Behavior: Returns true, advances index, and captures the following value string.
 */
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
