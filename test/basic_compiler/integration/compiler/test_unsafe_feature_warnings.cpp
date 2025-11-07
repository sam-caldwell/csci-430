// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: Integration.UnsafeFeatureWarnings_AppearInSemanticLog
 * Inputs: Program using CALL/DEF USR/USR/BLOAD/BSAVE/POKE/PEEK/CHDIR.
 * Code under test: Compiler::compileStringWithPhaseLogs semantic warning emission.
 * Expected behavior: Semantic log contains warnings for each unsafe feature.
 */
TEST(Integration, UnsafeFeatureWarnings_AppearInSemanticLog) {
    namespace fs = std::filesystem;
    const auto src =
        "10 CALL 100\n"
        "20 DEF USR0 = 100\n"
        "30 PRINT USR(1)\n"
        "40 BLOAD \"f\", 0\n"
        "50 BSAVE \"f\", 0, 1\n"
        "60 POKE 100, 1\n"
        "70 PRINT PEEK(100)\n"
        "80 CHDIR \".\"\n"
        "90 END\n";

    const fs::path outdir = fs::current_path() / "unsafe_warns";
    fs::create_directories(outdir);
    const fs::path sem = outdir / "semantic.log";
    std::string ir = Compiler::compileStringWithPhaseLogs(src, (outdir/"lex.log").string(), (outdir/"syn.log").string(), sem.string(), (outdir/"cg.log").string());
    ASSERT_FALSE(ir.empty());

    std::ifstream in(sem);
    std::string log((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_NE(log.find("Warning: unsafe CALL"), std::string::npos);
    EXPECT_NE(log.find("Warning: unsafe DEF USR"), std::string::npos);
    EXPECT_NE(log.find("Warning: unsafe USR"), std::string::npos);
    EXPECT_NE(log.find("Warning: unsafe BLOAD"), std::string::npos);
    EXPECT_NE(log.find("Warning: unsafe BSAVE"), std::string::npos);
    EXPECT_NE(log.find("Warning: unsafe POKE"), std::string::npos);
    EXPECT_NE(log.find("Warning: unsafe PEEK"), std::string::npos);
    EXPECT_NE(log.find("Warning: unsafe CHDIR"), std::string::npos);
}
