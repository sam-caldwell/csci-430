// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenFor.NegativeStepAndPrints
 * Purpose: Cover negative STEP in FOR and both integer/float print branches.
 * Components Under Test: CodeGenerator emitFor
 * Expected Behavior: IR contains for_cond/body/labels and integer detection branch labels.
 */
TEST(CodeGenFor, NegativeStepAndPrints) {
    const auto src =
        "10 FOR I% = 5 TO 1 STEP -2\n"
        "20 PRINT I%, I%+0.5\n"
        "30 NEXT I%\n"
        "40 END\n";
    const std::string ir = Compiler::compileString(src);
    // Basic FOR structure
    ASSERT_NE(ir.find("_for_cond"), std::string::npos);
    ASSERT_NE(ir.find("_for_body"), std::string::npos);
    ASSERT_NE(ir.find("_for_inc"), std::string::npos);
    // Integer/float selection is used in FOR Printing
    ASSERT_NE(ir.find("_print_int_"), std::string::npos);
    ASSERT_NE(ir.find("_print_flt_"), std::string::npos);
}
