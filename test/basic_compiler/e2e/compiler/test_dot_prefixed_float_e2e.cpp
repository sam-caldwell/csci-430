// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/*
 * Test: E2E.DotPrefixedFloat_ArithmeticAndPrint
 * Inputs: Program printing .5 + .25
 * Code under test: Full pipeline (lexer tolerates leading '.', parser, codegen, runtime)
 * Expected behavior: Prints 0.750000 (float format) with newline
 */
TEST(E2E, DotPrefixedFloat_ArithmeticAndPrint) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const std::string src =
        "10 PRINT .5 + .25\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_dotfloat";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll"; auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    ASSERT_EQ(out, std::string("0.750000\n"));
}

