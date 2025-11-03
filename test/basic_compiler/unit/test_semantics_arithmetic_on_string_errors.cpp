// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Semantics.ArithmeticOnStringErrors
 * Purpose: Validate that arithmetic with a string operand is rejected by semantic analysis.
 * Components Under Test: Compiler::compileString; Semantics (type checking)
 * Expected Behavior: Compilation throws SemanticError for expression "\"A\" + 1".
 */
TEST(Semantics, ArithmeticOnStringErrors) {
    const auto src =
        "10 LET A = \"A\" + 1\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
