// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "basic_compiler/DeriveDefaultLogPaths.h"

/***
 * Test: CLI_DeriveDefaultLogs.RespectsProvidedValuesAndNoLogs
 * Purpose: Ensure provided log paths are preserved and no additional paths are set when noLogs=true.
 * Components Under Test: deriveDefaultLogPaths().
 * Expected Behavior: Only the pre-set codegen path remains; others are unset.
 */
TEST(CLI_DeriveDefaultLogs, RespectsProvidedValuesAndNoLogs) {
    std::optional<std::string> cg{"cg.log"}, lex, syn, sem;
    deriveDefaultLogPaths("prog.bas", true, cg, lex, syn, sem);
    // noLogs=true, nothing should change
    EXPECT_EQ(cg.value(), std::string("cg.log"));
    EXPECT_FALSE(lex.has_value());
    EXPECT_FALSE(syn.has_value());
    EXPECT_FALSE(sem.has_value());
}
