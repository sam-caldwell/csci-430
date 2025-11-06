// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/*
 * Test: E2E.CHR_ASC
 * Purpose: CHR$(65) prints "A" and ASC("A") prints 65 as a float string.
 */
/*
Test: E2E.CHR_ASC
Inputs: BASIC program(s) executed end-to-end (runtime output)
Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
Expected behavior: Program compiles and runs; output/behavior matches expectations
*/
TEST(E2E, CHR_ASC) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 PRINT CHR$(65)\n"
        "20 PRINT ASC(\"A\")\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_chr_asc";
    std::filesystem::create_directories(tmp);
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
    // Expect two lines: A and 65
    ASSERT_NE(out.find("A\n"), std::string::npos);
    ASSERT_NE(out.find("65\n"), std::string::npos);
}
