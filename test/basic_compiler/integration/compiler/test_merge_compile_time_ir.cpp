// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.Merge_CompileTime_AppendsLines
 * Purpose: Ensure MERGE is applied at compile time when compiling from a file
 * and that merged file literals appear in the final IR.
 */
TEST(Integration, Merge_CompileTime_AppendsLines) {
    namespace fs = std::filesystem;
    fs::path tmp = fs::path("..") / "tmp" / "gwbasic_merge_test";
    fs::create_directories(tmp);
    // Child program with two prints
    fs::path child = tmp / "child.bas";
    {
        std::ofstream f(child);
        f << "10 PRINT \"A\"\n";
        f << "20 PRINT \"B\"\n";
    }
    // Root program merges child and adds another print
    fs::path root = tmp / "root.bas";
    {
        std::ofstream f(root);
        f << "5 MERGE \"child.bas\"\n";
        f << "30 PRINT \"X\"\n";
    }
    std::string ir = Compiler::compileFile(root.string());
    // Expect IR to contain the merged literals and the root literal
    ASSERT_NE(ir.find("c\"A\\00\""), std::string::npos);
    ASSERT_NE(ir.find("c\"B\\00\""), std::string::npos);
    ASSERT_NE(ir.find("c\"X\\00\""), std::string::npos);
}

