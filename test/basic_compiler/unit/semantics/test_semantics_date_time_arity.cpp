// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsDateTime.ZeroArityAccepted
 * Purpose: DATE$()/TIME$() require zero args; bare DATE$/TIME$ as variables accepted too.
 */
TEST(SemanticsDateTime, ZeroArityAccepted) {
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 PRINT DATE$\n20 PRINT TIME$\n"); });
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 PRINT DATE$()\n20 PRINT TIME$()\n"); });
}

/***
 * Test: SemanticsDateTime.NonZeroArityRejected
 * Purpose: Calls with one arg should be rejected.
 */
TEST(SemanticsDateTime, NonZeroArityRejected) {
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT DATE$(1)\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT TIME$(2)\n"); }, SemanticError);
}

