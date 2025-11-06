// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.ApostropheComment_IgnoresRestOfLine
 * Purpose: Ensure apostrophe (') comments are treated as REM to EOL and
 *          do not lex/parse trailing garbage.
 * Components: Lexer, Parser, Codegen
 * Expected: IR contains print formatting for two numeric prints.
 */
/*
Test: Integration.ApostropheComment_IgnoresRestOfLine
Inputs: Program with inline apostrophe comments containing invalid tokens
Code under test: Full compile pipeline
Expected behavior: Compilation succeeds; IR has @.fmt_num references (>=2)
*/
TEST(Integration, ApostropheComment_IgnoresRestOfLine) {
    const char* src =
        "10 PRINT 1 ' !! $$$ invalid tokens after comment\n"
        "20 PRINT 2 ' more !@# junk\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    // Two numeric print format references should appear in IR
    size_t pos = 0; int count = 0; while ((pos = ir.find("@.fmt_num", pos)) != std::string::npos) { ++count; ++pos; }
    ASSERT_GE(count, 2);
}

