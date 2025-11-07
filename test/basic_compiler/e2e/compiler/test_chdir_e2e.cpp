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

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.CHDIR_BSAVETouchesFileInNewDir
 * Purpose: Change to a temp directory and BSAVE a zero-length file; verify file exists in that directory.
 */
TEST(E2E, CHDIR_BSAVETouchesFileInNewDir) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    std::filesystem::path base = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_chdir";
    std::filesystem::create_directories(base);
    auto abs = std::filesystem::absolute(base);
    std::string dir = abs.string();
    // Compose BASIC with absolute dir literal
    std::ostringstream src;
    src << "10 CHDIR \"" << dir << "\"\n";
    src << "20 BSAVE \"touch.bin\", 0, 0\n";
    src << "30 END\n";

    std::string ir = Compiler::compileString(src.str());
    ASSERT_FALSE(ir.empty());

    std::filesystem::path ll = base / "program.ll";
    std::filesystem::path bin = base / "program.out";
    { std::ofstream f(ll); f << ir; }

    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    int ec = std::system(c1.str().c_str());
    ASSERT_EQ(ec, 0);

    std::ostringstream r1; r1 << '"' << bin.string() << '"';
    std::string out = runCommand(r1.str());
    (void)out;

    std::filesystem::path touched = base / "touch.bin";
    ASSERT_TRUE(std::filesystem::exists(touched));
}
