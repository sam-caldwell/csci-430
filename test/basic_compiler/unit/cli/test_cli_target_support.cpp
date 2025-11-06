// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/TargetUtils.h"

TEST(CLI_TargetSupport, AcceptsCommonTriples) {
    EXPECT_TRUE(isSupportedTargetTriple("x86_64-apple-darwin"));
    EXPECT_TRUE(isSupportedTargetTriple("arm64-apple-macosx15.0.0"));
    EXPECT_TRUE(isSupportedTargetTriple("aarch64-linux-gnu"));
}

TEST(CLI_TargetSupport, RejectsUnknownTriples) {
    EXPECT_FALSE(isSupportedTargetTriple("riscv64-unknown-elf"));
    EXPECT_FALSE(isSupportedTargetTriple("mips64-unknown-linux-gnu"));
}

