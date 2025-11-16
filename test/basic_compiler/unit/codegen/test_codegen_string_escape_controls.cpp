// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenStrings.EscapeControlCharacters
 * Purpose: Cover escapeForIR branches for tab and carriage return.
 * Components Under Test: CodeGenerator::escapeForIR via global string emission.
 * Expected Behavior: IR global includes \09 (tab) and \0D (carriage return) escapes.
 */
TEST(CodeGenStrings, EscapeControlCharacters) {
    const std::string tab_cr = std::string("A\tB\rC");
    const std::string src = std::string("10 PRINT \"") + tab_cr + "\"\n";
    const std::string ir = Compiler::compileString(src);
    // Look for escapes for tab and carriage return in the global constant
    ASSERT_NE(ir.find("\\09"), std::string::npos);
    ASSERT_NE(ir.find("\\0D"), std::string::npos);
}
