// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: Integration.MemIO_IRContainsFileCalls
 * Purpose: Ensure BSAVE/BLOAD emit expected libc calls in IR.
 */
/*
Test: Integration.MemIO_IRContainsFileCalls
Inputs: BASIC snippet compiled through multiple stages
Code under test: Parser + Semantics + Codegen integration
Expected behavior: Emitted IR/state contains expected constructs and values
*/
TEST(Integration, MemIO_IRContainsFileCalls) {
    const char* src =
        "10 DEF SEG=0\n"
        "20 BSAVE \"f.bin\", 0, 10\n"
        "30 BLOAD \"f.bin\", 0\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    ASSERT_NE(ir.find("@fopen"), std::string::npos);
    ASSERT_NE(ir.find("@fwrite"), std::string::npos);
    ASSERT_NE(ir.find("@fread"), std::string::npos);
}

