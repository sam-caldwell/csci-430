// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "clang_path.h"
#include "run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

TEST(E2E, READ_OutOfDataTriggersHandler) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 ON ERROR GOTO 100\n"
        "20 DATA \"A\"\n"
        "30 READ S$\n"
        "40 READ T$\n"
        "100 PRINT \"O\"\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_read_out_of_data";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "program.ll";
    auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::ostringstream run; run << '"' << bin.string() << '"';
    std::string out = runCommand(run.str());
    ASSERT_NE(out.find("O\n"), std::string::npos);
}

