// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

/***
 * Test: Metrics.OptimizedIrInstructionCounts_EmptyWhenNoClang
 * Purpose: Verify optimizedIrInstructionCounts returns empty when no clang path is provided.
 * Components Under Test: Metrics::optimizedIrInstructionCounts.
 * Expected Behavior: Returns an empty vector.
 */
TEST(Metrics, OptimizedIrInstructionCounts_EmptyWhenNoClang) {
    const std::string ir = "define i32 @main(){\n  ret i32 0\n}\n";
    const std::vector<std::string> phases = {"-O0", "-O2"};
    auto res = Metrics::optimizedIrInstructionCounts(ir, /*clang_path=*/"", phases);
    EXPECT_TRUE(res.empty());
}
