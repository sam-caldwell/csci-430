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
 * Test: E2E.DEF_TYPE
 * Inputs: BASIC program(s) executed end-to-end (runtime output)
 * Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
 * Expected behavior: Program compiles and runs; output/behavior matches expectations
 */
TEST(E2E, DEF_TYPE) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 DEFSTR A\n"
        "20 DEFINT B\n"
        "30 LET A = \"ok\"\n"
        "40 LET B = 5\n"
        "50 PRINT A\n"
        "60 PRINT B\n"
        "70 END\n";
    // Write to a temp file and compile via compileFile to align with file-based resolution
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_def_type";
    std::filesystem::create_directories(tmp);
    std::filesystem::path bas = tmp / "program.bas";
    { std::ofstream f(bas); f << src; }
    std::string ir = Compiler::compileFile(bas.string().c_str());
    ASSERT_FALSE(ir.empty());

    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    std::string cmd = c1.str();
    int ec = std::system(cmd.c_str());
    ASSERT_EQ(ec, 0) << "Clang failed: " << cmd;

    std::ostringstream r1; r1 << '"' << bin.string() << '"';
    std::string out = runCommand(r1.str());
    ASSERT_NE(out.find("ok\n"), std::string::npos);
    ASSERT_NE(out.find("5\n"), std::string::npos);
}
