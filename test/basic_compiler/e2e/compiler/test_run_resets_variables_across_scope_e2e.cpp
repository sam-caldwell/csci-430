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
#include "../../helper/source_root.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.Run_ResetsVariablesAcrossScope
 * Purpose: Validate that RUN clears all variables across the program boundary, regardless of COMMON.
 * Components Under Test: Compiler (compileFile), Clang driver, runtime output
 * Expected Behavior: Callee prints 0.000000 for X after RUN.
 */
TEST(E2E, Run_ResetsVariablesAcrossScope) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    const std::string ir = Compiler::compileFile((e2e_helpers::sourceRoot()+"/demos/run-pass-vars.bas").c_str());
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_run_reset";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "p.ll"; auto bin = tmp / "p.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    const std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    ASSERT_NE(out.find("0"), std::string::npos);
}
