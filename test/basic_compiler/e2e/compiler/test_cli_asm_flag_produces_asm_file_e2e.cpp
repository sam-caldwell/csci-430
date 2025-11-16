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

static std::string cliBin_asm() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

/***
 * Test: CLI.AsmFlagProducesAsmFile
 * Purpose: Verify --asm writes an assembly file (.asm) and exits successfully.
 * Components Under Test: CLI driver (assembly emission), filesystem, end-to-end invocation.
 * Expected Behavior: Command exits 0 and .asm file exists with non-zero size.
 */
TEST(CLI, AsmFlagProducesAsmFile) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP() << "clang not found; skipping";
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_asm";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "p.bas";
    const fs::path asmOut = tmp / "p_out"; // CLI will append .asm
    const fs::path asmExpect = tmp / "p_out.asm";
    { std::ofstream f(bas); f << "10 PRINT 2\n"; }
    const std::string cmd = '"' + cliBin_asm() + '"' + " \"" + bas.string() + "\" --asm \"" + asmOut.string() + "\" --no-logs 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos) << out;
    ASSERT_TRUE(fs::exists(asmExpect));
    ASSERT_GT(fs::file_size(asmExpect), static_cast<uintmax_t>(0));
}
