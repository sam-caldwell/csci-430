// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
Test: E2E.WIDTH_SCRN_WrapsAtWidth
Inputs: WIDTH 40; print 40 'A' with no newline, then print 'B'; then print SCREEN(2,1)
Expected: SCREEN(2,1) is ASCII for 'B' (66) because the 41st char wraps to next line.
*/
TEST(E2E, WIDTH_SCRN_WrapsAtWidth) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    std::ostringstream src;
    src << "10 WIDTH 40\n"
        << "20 FOR I=1 TO 40: PRINT \"A\"; : NEXT I\n"
        << "30 PRINT \"B\";\n"
        << "40 PRINT SCREEN(2,1)\n";
    const std::string ir = Compiler::compileString(src.str());
    const std::filesystem::path ll = "e2e_width.ll"; const std::filesystem::path bin = "e2e_width.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    const std::string out = runCommand(std::string("./") + bin.string());
    // Last line should be "66\n"
    ASSERT_NE(out.find("66\n"), std::string::npos) << out;
}

