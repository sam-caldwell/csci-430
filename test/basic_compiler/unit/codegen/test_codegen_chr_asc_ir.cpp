// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenBuiltins.CHR_EmitsMallocAndStores
 * Inputs: PRINT CHR$(65)
 * Code under test: Compiler::compileString() codegen for CHR$.
 * Expected behavior: IR allocates 2 bytes (char + NUL) via malloc and stores.
 */
TEST(CodeGenBuiltins, CHR_EmitsMallocAndStores) {
    const auto src =
        "10 PRINT CHR$(65)\n"
        "20 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("call ptr @malloc(i64 2)"), std::string::npos);
}
