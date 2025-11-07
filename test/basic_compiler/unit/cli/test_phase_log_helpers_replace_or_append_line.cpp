// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "basic_compiler/compiler/PhaseLogHelpers.h"

using namespace gwbasic::phase_log_helpers;
using gwbasic::Program;
using gwbasic::Line;

/***
 * Test: PhaseLogHelpers.ReplaceOrAppendLine
 * Purpose: Confirm replaceOrAppendLine replaces an existing line or appends when not present.
 * Components Under Test: phase_log_helpers::replaceOrAppendLine().
 * Expected Behavior: Vector size reflects replace vs. append behavior.
 */
TEST(PhaseLogHelpers, ReplaceOrAppendLine) {
    Program p{}; Line l1{10,{}}; Line l2{10,{}}; Line l3{20,{}};
    replaceOrAppendLine(p, std::move(l1), false);
    ASSERT_EQ(p.lines.size(), 1u);
    replaceOrAppendLine(p, std::move(l2), true); // replaces line 10
    ASSERT_EQ(p.lines.size(), 1u);
    replaceOrAppendLine(p, std::move(l3), false);
    ASSERT_EQ(p.lines.size(), 2u);
}
