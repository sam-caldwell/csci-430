// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/FileOpenError.h"
#include "basic_compiler/compiler/PhaseLogHelpers.h"

using gwbasic::FileOpenError;
namespace ph = gwbasic::phase_log_helpers;

/***
 * Test: CompilerExceptions.TokenizeRootWithLogs_ThrowsFileOpenError
 * Purpose: Ensure tokenizeRootWithLogs throws FileOpenError when root file is missing.
 * Components Under Test: phase_log_helpers::tokenizeRootWithLogs() error handling.
 * Expected Behavior: Throws FileOpenError.
 */
TEST(CompilerExceptions, TokenizeRootWithLogs_ThrowsFileOpenError) {
    const std::string missing = "/tmp/tokenize_root_missing.bas";
    std::string canon; int minLine = 0;
    EXPECT_THROW({ (void)ph::tokenizeRootWithLogs(missing, "/tmp/lex.log", "/tmp/syn.log", canon, minLine); }, FileOpenError);
}
