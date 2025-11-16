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
 * Test: E2E.Input_VarList_WithoutPrompt
 * Purpose: Verify console INPUT supports variable lists without a prompt and sums inputs.
 * Components Under Test: Parser (INPUT var[,var...]), Semantics (numeric-only console INPUT), Codegen (scanf list), End-to-End run.
 * Expected Behavior: Program reads two numbers from stdin and prints their sum.
 */
TEST(E2E, Input_VarList_WithoutPrompt) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const char* src =
        "10 INPUT N, M\n"
        "20 PRINT N+M\n";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_input_prompt_list";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "program.ll"; auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cc; cc << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cc << " -lm";
#endif
    ASSERT_EQ(std::system(cc.str().c_str()), 0);
    std::string out = runCommandWithInput(bin.string(), "2 3\\n");
    ASSERT_NE(out.find("5\n"), std::string::npos);
}
