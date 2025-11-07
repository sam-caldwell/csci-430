// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "basic_compiler/cli/TakeOptValue.h"

using namespace gwbasic::cli;

/***
 * Test: CLI_TakeOptValue.SingleName_NoMatchOrMissingValue
 * Purpose: Ensure non-matching option returns false and missing value does not consume.
 * Components Under Test: cli::takeOptValue(const char*, const char*, ...).
 * Expected Behavior: Returns false and index remains unchanged when no value is present.
 */
TEST(CLI_TakeOptValue, SingleName_NoMatchOrMissingValue) {
    const char* argv[] = {"prog", "--ll"};
    constexpr int argc = 2;
    int i = 1;
    std::optional<std::string> out;
    EXPECT_FALSE(takeOptValue(argv[i], "--bc", i, argc, const_cast<char**>(argv), out));
    EXPECT_FALSE(out.has_value());
    // Missing value should not consume
    EXPECT_FALSE(takeOptValue(argv[i], "--ll", i, argc, const_cast<char**>(argv), out));
    EXPECT_EQ(i, 1);
}
