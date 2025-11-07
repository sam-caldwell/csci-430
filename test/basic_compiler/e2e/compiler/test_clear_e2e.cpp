// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.CLEAR_ResetsScalars
 * Purpose: Validate runtime behavior: CLEAR resets previously set variables to zero.
 * Expected: Output shows zeros after CLEAR.
 */
TEST(E2E, CLEAR_ResetsScalars) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    std::string src =
        "10 A=5: B=6\n"
        "20 CLEAR\n"
        "30 PRINT A,B\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_clear";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c2; c2 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c2 << " -lm";
#endif
    int ec = std::system(c2.str().c_str());
    ASSERT_EQ(ec, 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    ASSERT_NE(out.find("0 0\n"), std::string::npos);
}
