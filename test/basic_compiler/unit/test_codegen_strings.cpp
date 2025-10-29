// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: CodeGen Strings
 * Purpose: Verify emission of interned global string literals and correctness
 *          of LLVM IR escape sequences used in globals.
 * Components Under Test: CodeGenerator emitGlobals, escapeForIR,
 *          PRINT string lowering.
 * Expected Behavior: Presence of @.str.N constants with expected escapes,
 *          and GEP to @.fmt_str for string printing.
 */
TEST(CodeGenStrings, EscapesCommonCharactersInGlobals) {
    // Include tab, newline, quote and backslash: A\tB\nC\"\\D
    const auto src = "10 PRINT \"A\\tB\\nC\\\"\\\\D\"\n20 END\n";
    std::string ir = Compiler::compileString(src);
    // Expect the escaped payload in the global string literal
    // A\09B\0AC\22\5CD\00 (plus header around it)
    EXPECT_NE(ir.find("@.str.0 = private unnamed_addr constant"), std::string::npos);
    EXPECT_NE(ir.find("c\"A\\09B\\0AC\\22\\5CD\\00\""), std::string::npos);
}
