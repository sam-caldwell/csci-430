// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

/*
Test: Metrics.CountIrInstructions_IgnoresNonInstructions
Inputs: Synthetic IR text with labels, comments, metadata, attributes, declares, targets, and instructions
Code under test: Metrics::countIrInstructions
Expected behavior: Counts only instruction lines (indented), ignores non-instruction rows
*/
TEST(Metrics, CountIrInstructions_IgnoresNonInstructions) {
    const char* ir = R"IR(
; ModuleID = 'test'
source_filename = "test.ll"
target triple = "x86_64-unknown-linux-gnu"
attributes #0 = { nounwind }
declare void @puts(ptr)
!0 = !{i32 42}

define i32 @main() {
entry:
  %a = add i32 1, 2
  call void @puts(ptr @.str)
  ret i32 0
}
)IR";
    const std::size_t n = Metrics::countIrInstructions(ir);
    // Expect 3 instructions inside the function body
    EXPECT_EQ(n, static_cast<std::size_t>(3));
}

/*
Test: Metrics.OptimizedIrInstructionCounts_EmptyWhenNoClang
Inputs: Non-empty IR and phases but empty clang path
Code under test: Metrics::optimizedIrInstructionCounts
Expected behavior: Returns empty results when clang path is empty
*/
TEST(Metrics, OptimizedIrInstructionCounts_EmptyWhenNoClang) {
    const std::string ir = "define i32 @main(){\n  ret i32 0\n}\n";
    const std::vector<std::string> phases = {"-O0", "-O2"};
    auto res = Metrics::optimizedIrInstructionCounts(ir, /*clang_path=*/"", phases);
    EXPECT_TRUE(res.empty());
}
