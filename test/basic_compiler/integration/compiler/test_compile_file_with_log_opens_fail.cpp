// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/compiler/FileOpenError.h"

using namespace gwbasic;

/***
 * Test: Compiler.CompileFileWithLog_OpensFail
 * Purpose: Verify compileFileWithLog throws when the source file cannot be opened.
 * Components Under Test: Compiler::compileFileWithLog() error path, FileOpenError.
 * Expected Behavior: Throws FileOpenError.
 */
TEST(Compiler, CompileFileWithLog_OpensFail) {
    EXPECT_THROW({ (void)Compiler::compileFileWithLog("/no/such/file.bas", "out.log"); }, FileOpenError);
}
