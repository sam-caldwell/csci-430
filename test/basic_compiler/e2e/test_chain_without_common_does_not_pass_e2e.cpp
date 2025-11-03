// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.Chain_WithoutCommon_DoesNotPass
 * Purpose: Validate that CHAIN without COMMON in the caller does not pass variables to the callee.
 * Components Under Test: Compiler (compileFile), Clang driver, runtime output
 * Expected Behavior: Callee prints 0.000000 for X when caller lacks COMMON X.
 */
TEST(E2E, Chain_WithoutCommon_DoesNotPass) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    std::string ir = Compiler::compileFile("demos/pass-params-nocommon.bas");
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_chain_nocommon";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "p.ll"; auto bin = tmp / "p.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    ASSERT_NE(out.find("0.000000"), std::string::npos);
}

