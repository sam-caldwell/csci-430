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

TEST(E2E, COLOR_SetsAnsiCodes) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 COLOR 2,4\n"
        "20 PRINT \"X\"\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_color";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll";
    auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    int ec = std::system(c1.str().c_str());
    ASSERT_EQ(ec, 0);
    std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    // Expect ESC[32m (green fg) and ESC[41m (red bg)
    ASSERT_NE(out.find("\033[32m"), std::string::npos);
    ASSERT_NE(out.find("\033[41m"), std::string::npos);
}

