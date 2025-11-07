// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/TargetUtils.h"

/***
 * Test: CLI_TargetSupport.AcceptsCommonTriples
 * Purpose: Verify common target triples are recognized as supported.
 * Components Under Test: isSupportedTargetTriple().
 * Expected Behavior: Returns true for known supported triples.
 */
TEST(CLI_TargetSupport, AcceptsCommonTriples) {
    EXPECT_TRUE(isSupportedTargetTriple("x86_64-apple-darwin"));
    EXPECT_TRUE(isSupportedTargetTriple("arm64-apple-macosx15.0.0"));
    EXPECT_TRUE(isSupportedTargetTriple("aarch64-linux-gnu"));
}
