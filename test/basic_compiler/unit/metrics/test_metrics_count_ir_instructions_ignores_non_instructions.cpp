// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

/***
 * Test: Metrics.CountIrInstructions_IgnoresNonInstructions
 * Purpose: Ensure non-instruction lines (labels, decls, metadata) are ignored by the instruction counter.
 * Components Under Test: Metrics::countIrInstructions.
 * Expected Behavior: Counts only indented instruction lines inside functions.
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
