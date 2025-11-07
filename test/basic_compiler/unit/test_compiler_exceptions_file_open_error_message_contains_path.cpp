// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/FileOpenError.h"

using gwbasic::FileOpenError;

/***
 * Test: CompilerExceptions.FileOpenError_MessageContainsPath
 * Purpose: Verify FileOpenError::what() message includes the offending path.
 * Components Under Test: FileOpenError.
 * Expected Behavior: Error message contains the input path string.
 */
TEST(CompilerExceptions, FileOpenError_MessageContainsPath) {
    const std::string p = "/tmp/definitely_not_here.bas";
    FileOpenError ex(p);
    std::string msg = ex.what();
    ASSERT_NE(msg.find(p), std::string::npos);
}
