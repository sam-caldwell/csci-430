// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/FileOpenError.h"
#include "basic_compiler/compiler/PhaseLogHelpers.h"

using gwbasic::FileOpenError;
namespace ph = gwbasic::phase_log_helpers;

/***
 * Test: CompilerExceptions.ParseFileNoLogs_ThrowsFileOpenError
 * Purpose: Ensure parseFileNoLogs throws FileOpenError for invalid path.
 * Components Under Test: phase_log_helpers::parseFileNoLogs() error handling.
 * Expected Behavior: Throws FileOpenError.
 */
TEST(CompilerExceptions, ParseFileNoLogs_ThrowsFileOpenError) {
    const std::string missing = "/tmp/parse_no_logs_missing.bas";
    EXPECT_THROW({ (void)ph::parseFileNoLogs(missing); }, FileOpenError);
}
