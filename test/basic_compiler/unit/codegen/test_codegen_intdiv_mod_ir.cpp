// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGen.IntDivAndMod_IR
 * Inputs: BASIC using integer division (\\) and MOD
 * Code under test: LLVM IR code generator
 * Expected behavior: Emits sdiv/srem on integer operands
 */
TEST(CodeGen, IntDivAndMod_IR) {
    const auto src =
        "10 PRINT 5 \\ 2\n"
        "20 PRINT 5 MOD 2\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    // Expect integer operations present
    ASSERT_NE(ir.find("sdiv i64"), std::string::npos);
    ASSERT_NE(ir.find("srem i64"), std::string::npos);
}
