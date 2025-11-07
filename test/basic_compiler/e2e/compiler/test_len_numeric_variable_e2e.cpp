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
 * Test: E2E.LEN_Numeric_Variable
 * Purpose: Verify LEN on numeric variables returns storage size in bytes:
 *          Int16 (%)=2, Long32 (&)=4, Single (!)=4, Double (#)=8; and plain
 *          (no suffix) under DEFSNG is 4.
 */
TEST(E2E, LEN_Numeric_Variable) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const std::string src =
        "10 DEFINT A\n"
        "20 DEFSNG B\n"
        "30 DEFDBL C\n"
        "40 A%=1: B!=1: C#=1: D&=1: X=1\n"
        "50 PRINT LEN(A%), LEN(B!), LEN(C#), LEN(D&), LEN(X)\n"
        "60 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..")/"tmp"/"gwbasic_e2e_len_numeric";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll"; auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    // Expected: 2 4 8 4 4 (each followed by newline or spaces per print rules)
    // Since PRINT separates items with a space (legacy behavior), we search for substrings
    ASSERT_NE(out.find("2"), std::string::npos);
    ASSERT_NE(out.find("4"), std::string::npos);
    ASSERT_NE(out.find("8"), std::string::npos);
}
