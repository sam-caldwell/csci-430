// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test Suite: Semantics FOR type checks
 */
TEST(Semantics, ForStartMustBeNumeric) {
    const auto src =
        "10 FOR I=\"A\" TO 3:NEXT I\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(Semantics, ForEndMustBeNumeric) {
    const auto src =
        "10 FOR I=1 TO \"Z\":NEXT I\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(Semantics, ForStepMustBeNumeric) {
    const auto src =
        "10 FOR I=1 TO 3 STEP \"X\":NEXT I\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

