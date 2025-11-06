// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/compiler/FileOpenError.h"

using namespace gwbasic;

/*
 * Test: Compiler.CompileFileWithLog_Success
 * Purpose: Ensure compileFileWithLog reads source file, writes a log, and returns IR.
 */
TEST(Compiler, CompileFileWithLog_Success) {
    namespace fs = std::filesystem;
    // Create a temporary source file in the build directory
    fs::path tmpDir = fs::current_path();
    fs::path srcPath = tmpDir / "tmp_compile_success.bas";
    fs::path logPath = tmpDir / "tmp_compile_success.log";
    {
        std::ofstream out(srcPath);
        out << "10 PRINT \"OK\"\n20 END\n";
    }
    std::string ir = Compiler::compileFileWithLog(srcPath.string(), logPath.string());
    ASSERT_FALSE(ir.empty());
    // Clean up temp files
    std::error_code ig;
    fs::remove(srcPath, ig);
    fs::remove(logPath, ig);
}

/*
 * Test: Compiler.CompileFileWithLog_OpensFail
 * Purpose: Verify compileFileWithLog throws on missing file.
 */
TEST(Compiler, CompileFileWithLog_OpensFail) {
    EXPECT_THROW({ (void)Compiler::compileFileWithLog("/no/such/file.bas", "out.log"); }, FileOpenError);
}
