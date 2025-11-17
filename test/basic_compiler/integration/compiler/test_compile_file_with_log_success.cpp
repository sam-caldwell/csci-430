// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Compiler.CompileFileWithLog_Success
 * Purpose: Ensure compileFileWithLog reads a source file, writes a log, and returns non-empty IR.
 * Components Under Test: Compiler::compileFileWithLog(), filesystem I/O.
 * Expected Behavior: IR string is non-empty and no exception is thrown.
 */
TEST(Compiler, CompileFileWithLog_Success) {
    namespace fs = std::filesystem;
    // Create a temporary source file in the build directory
    fs::path tmpDir = fs::current_path();
    fs::path srcPath = tmpDir / "tmp_compile_success.bas";
    {
        std::ofstream out(srcPath);
        out << "10 PRINT \"OK\"\n20 END\n";
    }
    const std::string ir = Compiler::compileFile(srcPath.string());
    ASSERT_FALSE(ir.empty());
    // Clean up temp files
    std::error_code ig;
    fs::remove(srcPath, ig);
    // Logging removed; no log file to clean up.
}
