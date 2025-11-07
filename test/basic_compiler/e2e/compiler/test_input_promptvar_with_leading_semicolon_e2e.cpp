// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_with_input.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.Input_PromptVar_WithLeadingSemicolon
 * Purpose: Ensure INPUT with leading semicolon and prompt variable prints the prompt and reads a number.
 * Components Under Test: Parser (INPUT ; P$, var), Semantics (prompt var/string handling), Codegen (printf prompt + scanf), End-to-End run.
 * Expected Behavior: Output contains the prompt text and the entered number is printed.
 */
TEST(E2E, Input_PromptVar_WithLeadingSemicolon) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const char* src =
        "10 P$=\"Enter:\"\n"
        "20 INPUT ; P$, X\n"
        "30 PRINT X\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_input_prompt_var";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "program.ll"; auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cc; cc << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cc << " -lm";
#endif
    ASSERT_EQ(std::system(cc.str().c_str()), 0);
    std::string out = runCommandWithInput(bin.string(), "7\\n");
    ASSERT_NE(out.find("Enter:"), std::string::npos);
    ASSERT_NE(out.find("7\n"), std::string::npos);
}
