// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "basic_compiler/DeriveDefaultLogPaths.h"

TEST(CLI_DeriveDefaultLogs, PopulatesWhenMissingAndNoLogsFalse) {
    std::optional<std::string> cg, lex, syn, sem;
    deriveDefaultLogPaths("/tmp/prog.bas", false, cg, lex, syn, sem);
    ASSERT_TRUE(cg.has_value());
    ASSERT_TRUE(lex.has_value());
    ASSERT_TRUE(syn.has_value());
    ASSERT_TRUE(sem.has_value());
    EXPECT_NE(cg->find(".codegen.log"), std::string::npos);
    EXPECT_NE(lex->find(".lex.log"), std::string::npos);
    EXPECT_NE(syn->find(".syntax.log"), std::string::npos);
    EXPECT_NE(sem->find(".semantic.log"), std::string::npos);
}

TEST(CLI_DeriveDefaultLogs, RespectsProvidedValuesAndNoLogs) {
    std::optional<std::string> cg{"cg.log"}, lex, syn, sem;
    deriveDefaultLogPaths("prog.bas", true, cg, lex, syn, sem);
    // noLogs=true, nothing should change
    EXPECT_EQ(cg.value(), std::string("cg.log"));
    EXPECT_FALSE(lex.has_value());
    EXPECT_FALSE(syn.has_value());
    EXPECT_FALSE(sem.has_value());
}

