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

TEST(E2E, READ_TypeMismatch_StringIntoNumericTriggersHandler) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    std::string src = R"(10 ON ERROR GOTO 100
20 DATA "X"
30 READ A
40 PRINT 1
100 PRINT "E"
)";
    std::string ir = Compiler::compileString(src);
    std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_read_type_mismatch";
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
    // Only handler message should appear
    ASSERT_NE(out.find("E\n"), std::string::npos);
    ASSERT_EQ(out.find("1\n"), std::string::npos);
}

