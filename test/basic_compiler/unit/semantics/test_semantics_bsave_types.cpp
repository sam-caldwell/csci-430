// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsMemIO.BsaveTypes
 * Inputs: BSAVE "f", "X", 10 (non-numeric address)
 * Code under test: Compiler::compileString() + semantic checks for BSAVE.
 * Expected behavior: SemanticError thrown due to invalid type for address.
 */
TEST(SemanticsMemIO, BsaveTypes) {
    const char* src =
        "10 BSAVE \"f\", \"X\", 10\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

