// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.CLS_ClearsVirtualScreen
 * Purpose: After printing a character, CLS should clear the virtual screen; SCREEN(1,1) should return 0.
 */
TEST(E2E, CLS_ClearsVirtualScreen) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const std::string src =
        "10 PRINT \"A\"\n"
        "20 CLS\n"
        "30 PRINT SCREEN(1,1)\n";
    std::string ir = Compiler::compileString(src.c_str());
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_cls";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "p.ll"; auto bin = tmp / "p.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    // After CLS, first screen cell should be 0; printed value should end with "0\n"
    ASSERT_NE(out.rfind("0"), std::string::npos);
}

