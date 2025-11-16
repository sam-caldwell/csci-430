// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "../../helper/source_root.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/clang_path.h"

using namespace e2e_helpers;

static std::string cliBin_bc() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.BitcodeFlagProducesBCFile
 * Purpose: Confirm --bc emits a non-empty LLVM bitcode file.
 * Components Under Test: CLI driver (bitcode emission), file system side effects, end-to-end invocation.
 * Expected Behavior: Command exits 0 and target .bc file exists with size > 0.
 */
TEST(CLI, BitcodeFlagProducesBCFile) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP() << "clang not found; skipping";
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_bc";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "p.bas";
    const fs::path bc  = tmp / "p.bc";
    { std::ofstream f(bas); f << "10 PRINT 1\n"; }
    const std::string cmd = '"' + cliBin_bc() + '"' + " \"" + bas.string() + "\" --bc \"" + bc.string() + "\" --no-logs 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos) << out;
    ASSERT_TRUE(fs::exists(bc));
    ASSERT_GT(fs::file_size(bc), static_cast<uintmax_t>(0));
}
