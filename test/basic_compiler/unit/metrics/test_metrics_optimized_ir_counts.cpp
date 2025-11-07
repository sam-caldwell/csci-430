// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

/***
 * Test: Metrics.OptimizedIrInstructionCounts_RunsPhases
 * Inputs: Tiny IR and two phases; provide a non-empty clang path
 * Code under test: Metrics::optimizedIrInstructionCounts
 * Expected behavior: Returns an entry per phase (counts may be zero if clang invocation fails)
 */
TEST(Metrics, OptimizedIrInstructionCounts_RunsPhases) {
    const std::string ir = "define i32 @main(){\n  ret i32 0\n}\n";
    const std::vector<std::string> phases = {"-O0", "-O2"};
    // Prefer a common clang path; fall back to 'clang'
#ifdef CLANG_PATH
    const std::string clang_path = CLANG_PATH;
#else
    const std::string clang_path = "clang";
#endif
    auto res = Metrics::optimizedIrInstructionCounts(ir, clang_path, phases);
    ASSERT_EQ(res.size(), phases.size());
}
