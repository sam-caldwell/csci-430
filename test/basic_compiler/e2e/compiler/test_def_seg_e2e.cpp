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
 * Test: E2E.DEF_SEG
 * Purpose: DEF SEG compiles (no-op) and program runs normally.
 */
TEST(E2E, DEF_SEG) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 DEF SEG = 0\n"
        "20 POKE 100, 65\n"
        "30 PRINT PEEK(100)\n"
        "40 BSAVE \"mem.bin\", 100, 1\n"
        "50 POKE 100, 0\n"
        "60 BLOAD \"mem.bin\", 100\n"
        "70 PRINT PEEK(100)\n"
        "80 PRINT USR(7)\n"
        "90 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());

    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_def_seg";
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
    ASSERT_NE(out.find("65\n"), std::string::npos);
    ASSERT_NE(out.find("7\n"), std::string::npos);
}
