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

/*
 * Test: E2E.Chain_All_PassesVariable
 * Inputs: BASIC program(s) executed end-to-end (runtime output)
 * Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
 * Expected behavior: Program compiles and runs; output/behavior matches expectations
*/
TEST(E2E, Chain_All_PassesVariable) {
    if (!toolExists(CLANG_PATH)) { GTEST_SKIP() << "clang not found"; }
    std::string ir = Compiler::compileFile((e2e_helpers::sourceRoot()+"/demos/pass-params-all.bas").c_str());
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_chain_all";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "p.ll"; auto bin = tmp / "p.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    ASSERT_NE(out.find("77"), std::string::npos);
}
