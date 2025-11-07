// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/FileOpenError.h"
#include "basic_compiler/Compiler.h"

using gwbasic::FileOpenError;
using gwbasic::Compiler;

/***
 * Test: CompilerExceptions.CompileFileWithLog_ThrowsFileOpenError
 * Purpose: Ensure compileFileWithLog throws FileOpenError when the source path is invalid.
 * Components Under Test: Compiler::compileFileWithLog() error handling.
 * Expected Behavior: Throws FileOpenError.
 */
TEST(CompilerExceptions, CompileFileWithLog_ThrowsFileOpenError) {
    const std::string missing = "/tmp/compile_file_with_log_missing.bas";
    EXPECT_THROW({ (void)Compiler::compileFileWithLog(missing, "/tmp/codegen.log"); }, FileOpenError);
}
