// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "run_command.h"
#include "../../helper/tool_exists.h"
#include "clang_path.h"

using namespace e2e_helpers;

static std::string cliBin() { return std::string("basic_compiler/basic_compiler"); }

TEST(CLI, BitcodeFlagProducesBCFile) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP() << "clang not found; skipping";
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_bc";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "p.bas";
    const fs::path bc  = tmp / "p.bc";
    { std::ofstream f(bas); f << "10 PRINT 1\n"; }
    const std::string cmd = '"' + cliBin() + '"' + " \"" + bas.string() + "\" --bc \"" + bc.string() + "\" --no-logs 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos) << out;
    ASSERT_TRUE(fs::exists(bc));
    ASSERT_GT(fs::file_size(bc), static_cast<uintmax_t>(0));
}

TEST(CLI, AsmFlagProducesAsmFile) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP() << "clang not found; skipping";
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_asm";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "p.bas";
    const fs::path asmOut = tmp / "p_out"; // CLI will append .asm
    const fs::path asmExpect = tmp / "p_out.asm";
    { std::ofstream f(bas); f << "10 PRINT 2\n"; }
    const std::string cmd = '"' + cliBin() + '"' + " \"" + bas.string() + "\" --asm \"" + asmOut.string() + "\" --no-logs 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos) << out;
    ASSERT_TRUE(fs::exists(asmExpect));
    ASSERT_GT(fs::file_size(asmExpect), static_cast<uintmax_t>(0));
}

TEST(CLI, LinkExecutableAndRun) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP() << "clang not found; skipping";
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_link";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "p.bas";
    const fs::path exe = tmp / "prog.out";
    { std::ofstream f(bas); f << "10 PRINT 9\n20 END\n"; }
    const std::string cmd = '"' + cliBin() + '"' + " \"" + bas.string() + "\" -o \"" + exe.string() + "\" --no-logs 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos) << out;
    ASSERT_TRUE(fs::exists(exe));
    const std::string runOut = runCommand('"' + exe.string() + '"');
    ASSERT_NE(runOut.find("9\n"), std::string::npos);
}

