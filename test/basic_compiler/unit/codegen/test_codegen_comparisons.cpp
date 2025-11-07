// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/compile_single_line_print.h"

using namespace gwbasic;

/***
 * Test Suite: CodeGen Comparisons
 * Purpose: Ensure each relational operator maps to the correct LLVM fcmp
 *          predicate in generated IR.
 * Components Under Test: CodeGenerator::emitComparison via Compiler facade.
 * Expected Behavior: Operators (=, <>, <, <=, >, >=) lower to oeq, one, olt,
 *          ole, ogt, oge respectively; operands appear in the IR as doubles.
 */
TEST(CodeGenComparisons, EqNeLtLeGtGePredicates) {
    std::string ir = compileSingleLinePrint("1 = 2");
    EXPECT_NE(ir.find(" = fcmp oeq double 1.0, 2.0"), std::string::npos);
    ir = compileSingleLinePrint("1 <> 2");
    EXPECT_NE(ir.find(" = fcmp one double 1.0, 2.0"), std::string::npos);
    ir = compileSingleLinePrint("1 < 2");
    EXPECT_NE(ir.find(" = fcmp olt double 1.0, 2.0"), std::string::npos);
    ir = compileSingleLinePrint("1 <= 2");
    EXPECT_NE(ir.find(" = fcmp ole double 1.0, 2.0"), std::string::npos);
    ir = compileSingleLinePrint("3 > 2");
    EXPECT_NE(ir.find(" = fcmp ogt double 3.0, 2.0"), std::string::npos);
    ir = compileSingleLinePrint("3 >= 2");
    EXPECT_NE(ir.find(" = fcmp oge double 3.0, 2.0"), std::string::npos);
}
