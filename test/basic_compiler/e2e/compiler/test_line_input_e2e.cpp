// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_with_input.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.LineInput_ReadsLineAndPrints
 * Purpose: Verify LINE INPUT reads a full line from stdin and PRINT echoes it.
 * Components Under Test: Parser (LINE INPUT), Codegen (fgets/scanf path), End-to-End run.
 * Expected Behavior: Program prints the entered line exactly once.
 */
TEST(E2E, LineInput_ReadsLineAndPrints) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    const auto src =
        "10 LINE INPUT A$\n"
        "20 PRINT A$\n";
    const std::string ir = Compiler::compileString(src);
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_line_input";
    std::filesystem::create_directories(tmp);
    auto ll = tmp / "program.ll"; auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream cc; cc << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cc << " -lm";
#endif
    ASSERT_EQ(std::system(cc.str().c_str()), 0);
    const std::string out = runCommandWithInput(bin.string(), "hello world\\n");
    ASSERT_NE(out.find("hello world\n"), std::string::npos);
}
