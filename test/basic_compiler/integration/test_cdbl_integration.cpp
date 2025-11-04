// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: Integration.CDBL_CompilesToNoopCast
 * Purpose: Ensure CDBL compiles and produces IR without extra helpers.
 * Notes: CDBL is a no-op cast to double in this compiler; we only assert basic IR presence.
 */
/*
Test: Integration.CDBL_CompilesToNoopCast
Inputs: BASIC snippet compiled through multiple stages
Code under test: Parser + Semantics + Codegen integration
Expected behavior: Emitted IR/state contains expected constructs and values
*/
TEST(Integration, CDBL_CompilesToNoopCast) {
    const char* src =
        "10 PRINT CDBL(2)\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    // Presence of printf is expected due to PRINT
    ASSERT_NE(ir.find("declare i32 @printf(ptr, ...)"), std::string::npos);
}
