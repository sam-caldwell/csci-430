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
 * Test: E2E.DELETE_RemovesLinesFromListing
 * Purpose: Compile and run a program with DELETE; LIST should not show
 *          deleted lines.
 */
TEST(E2E, DELETE_RemovesLinesFromListing) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 PRINT \"x\"\n"
        "20 DELETE 10-20\n"
        "30 LIST\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_delete";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll";
    auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    const int ec = std::system(c1.str().c_str());
    ASSERT_EQ(ec, 0);
    const std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    // Only line 30 should remain in the listing
    EXPECT_EQ(out.find("10\n"), std::string::npos);
    EXPECT_EQ(out.find("20\n"), std::string::npos);
    ASSERT_NE(out.find("30\n"), std::string::npos);
}

/***
 * Test: E2E.DELETE_NoopWhenNoMatch
 * Purpose: DELETE of a non-existent range should not affect listing.
 */
TEST(E2E, DELETE_NoopWhenNoMatch) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const char* src =
        "10 PRINT \"x\"\n"
        "20 LIST\n"
        "30 DELETE 9999-10000\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_delete2";
    std::filesystem::create_directories(tmp);
    auto ll = tmp/"program.ll";
    auto bin = tmp/"program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    const int ec = std::system(c1.str().c_str());
    ASSERT_EQ(ec, 0);
    const std::string out = runCommand(std::string("\"") + bin.string() + "\"");
    // Listing should include 10 and 20
    ASSERT_NE(out.find("10\n"), std::string::npos);
    ASSERT_NE(out.find("20\n"), std::string::npos);
}

