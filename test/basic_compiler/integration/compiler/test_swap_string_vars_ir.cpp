// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.SWAP_String_Vars_IR
 * Purpose: Ensure SWAP A$,B$ lowers to pointer loads/stores in IR.
 * Components Under Test: Parser (SWAP), Semantics (type check), Codegen (string var swap IR).
 * Expected Behavior: Generated IR includes load/store of ptr for both variables.
 */
TEST(Integration, SWAP_String_Vars_IR) {
    const auto src =
        "10 A$=\"X\"\n"
        "11 B$=\"Y\"\n"
        "20 SWAP A$,B$\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("load ptr, ptr %A$"), std::string::npos);
    ASSERT_NE(ir.find("load ptr, ptr %B$"), std::string::npos);
    ASSERT_NE(ir.find("store ptr %"), std::string::npos);
}
