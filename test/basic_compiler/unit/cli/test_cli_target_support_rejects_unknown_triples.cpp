// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/TargetUtils.h"

/***
 * Test: CLI_TargetSupport.RejectsUnknownTriples
 * Purpose: Ensure unknown target triples are reported as unsupported.
 * Components Under Test: isSupportedTargetTriple().
 * Expected Behavior: Returns false for unsupported triples.
 */
TEST(CLI_TargetSupport, RejectsUnknownTriples) {
    EXPECT_FALSE(isSupportedTargetTriple("riscv64-unknown-elf"));
    EXPECT_FALSE(isSupportedTargetTriple("mips64-unknown-linux-gnu"));
}
