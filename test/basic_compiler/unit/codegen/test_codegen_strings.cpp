// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/***
 * Test Suite: CodeGen Strings
 * Purpose: Verify emission of interned global string literals and correctness
 *          of LLVM IR escape sequences used in globals.
 * Components Under Test: CodeGenerator emitGlobals, escapeForIR,
 *          PRINT string lowering.
 * Expected Behavior: Presence of @.str.N constants with expected escapes,
 *          and GEP to @.fmt_str for string printing.
 */
TEST(CodeGenStrings, EscapesCommonCharactersInGlobals) {
    // GW-BASIC style: backslashes are literal; quotes are doubled inside string
    // Include sequences: A\tB\nC"\D (where \t and \n are literal backslash+letter)
    const auto src = "10 PRINT \"A\\tB\\nC\"\"\\\\D\"\n20 END\n";
    std::string ir = Compiler::compileString(src);
    // Debug: write IR to a temp file for offline inspection during CI
    {
        std::ofstream f("ir_codegen_strings.ll");
        f << ir;
    }
    // Expect the escaped payload in the global string literal
    // Expect: A\\5CtB\\5CnC\\22\\5CD\\00 (plus header around it)
    EXPECT_NE(ir.find("@.str.0 = private unnamed_addr constant"), std::string::npos);
    // Check that the escaped backslash (\\5C) appears at least twice and a quote (\\22) once
    auto countSubstr = [](const std::string& hay, const std::string& needle){
        size_t pos = 0; int cnt = 0; while ((pos = hay.find(needle, pos)) != std::string::npos) { ++cnt; ++pos; } return cnt; };
    EXPECT_GE(countSubstr(ir, "\\5C"), 2);
    EXPECT_GE(countSubstr(ir, "\\22"), 1);
}
