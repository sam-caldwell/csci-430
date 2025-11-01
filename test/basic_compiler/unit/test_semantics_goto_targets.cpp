// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test Suite: Semantics GOTO Target Validation
 */
TEST(Semantics, GotoValidTargetCompiles) {
    const auto src =
        "10 GOTO 30\n"
        "20 PRINT 1\n"
        "30 END\n";
    // Should compile without throwing (target exists)
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("line10:"), std::string::npos);
}

TEST(Semantics, GotoMissingTargetErrors) {
    const auto src =
        "10 GOTO 9999\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

