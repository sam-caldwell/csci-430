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

/***
 * Test: E2E.CALL_PrintsWhenOpcode1
 * Inputs: BASIC program(s) executed end-to-end (runtime output)
 * Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
 * Expected behavior: Program compiles and runs; output/behavior matches expectations
 */
TEST(E2E, CALL_PrintsWhenOpcode1) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    auto src =
        "10 POKE 100, 1\n"
        "20 CALL 100\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());

    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_call";
    std::filesystem::create_directories(tmp);
    const std::filesystem::path ll = tmp / "program.ll";
    const std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    std::string cmd = c1.str();
    const int ec = std::system(cmd.c_str());
    ASSERT_EQ(ec, 0) << "Clang failed: " << cmd;

    std::ostringstream r1; r1 << '"' << bin.string() << '"';
    const std::string out = runCommand(r1.str());
    ASSERT_NE(out.find("CALLED\n"), std::string::npos);
}
