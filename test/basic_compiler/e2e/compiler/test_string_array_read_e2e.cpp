// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/split_lines.h"

using namespace gwbasic;
using namespace e2e_helpers;

TEST(E2E, StringArray_READ_Works) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const auto src =
        "10 DIM A$(3)\n"
        "20 DATA \"ONE\",\"TWO\",\"THREE\"\n"
        "30 READ A$(1),A$(2),A$(3)\n"
        "40 PRINT A$(1)\n"
        "50 PRINT A$(2)\n"
        "60 PRINT A$(3)\n"
        "70 END\n";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_string_array_read";
    std::filesystem::create_directories(tmp);
    std::filesystem::path ll = tmp / "program.ll";
    std::filesystem::path bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    int ec = std::system(cmd.str().c_str());
    ASSERT_EQ(ec, 0);
    std::ostringstream run; run << '"' << bin.string() << '"';
    std::string out = runCommand(run.str());
    auto lines = splitLines(out);
    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0], std::string("ONE"));
    EXPECT_EQ(lines[1], std::string("TWO"));
    EXPECT_EQ(lines[2], std::string("THREE"));
}

