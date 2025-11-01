// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;
/*
 * Test Suite: E2E Compile and Run
 * Purpose: Exercise the full pipeline by compiling IR with clang and running
 *          the resulting binary to check observable behavior.
 * Components Under Test: Full compiler pipeline; external clang; runtime.
 * Expected Behavior: Compiled program prints expected numeric and string lines.
 */
TEST(E2E, CompileAndRun) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    std::string src = R"(10 LET A = 1+2*3
20 PRINT A
30 IF A > 100 THEN 50
40 PRINT "Done"
50 END
)";
    std::string ir = Compiler::compileString(src);

    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\""; std::string cmd = c1.str();
    int ec = std::system(cmd.c_str());
    ASSERT_EQ(ec, 0) << "Clang failed: " << cmd;

    std::ostringstream r1; r1 << '"' << bin.string() << '"'; std::string out = runCommand(r1.str());
    ASSERT_NE(out.find("7.000000\n"), std::string::npos);
    ASSERT_NE(out.find("Done\n"), std::string::npos);
}
