// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/FileOpenError.h"
#include "basic_compiler/Compiler.h"

using gwbasic::FileOpenError;
using gwbasic::Compiler;

/***
 * Test: CompilerExceptions.CompileFile_ThrowsFileOpenError
 * Purpose: Ensure compileFile throws FileOpenError when the source path is invalid.
 * Components Under Test: Compiler::compileFile() error handling.
 * Expected Behavior: Throws FileOpenError.
 */
TEST(CompilerExceptions, CompileFile_ThrowsFileOpenError) {
    const std::string missing = "/tmp/compile_file_missing.bas";
    EXPECT_THROW({ (void)Compiler::compileFile(missing); }, FileOpenError);
}
