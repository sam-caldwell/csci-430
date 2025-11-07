// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <unordered_map>
#include <string>
#include "basic_compiler/compiler/PhaseLogHelpers.h"

using namespace gwbasic::phase_log_helpers;
using gwbasic::Program;
using gwbasic::Line;

/***
 * Test: PhaseLogHelpers.AssignBaseAndRenumber
 * Purpose: Validate assignBase chooses a base offset and renumberProgram adjusts line numbers accordingly.
 * Components Under Test: phase_log_helpers::assignBase(), renumberProgram().
 * Expected Behavior: Same file yields base 0; other file yields positive base; renumber increases first line.
 */
TEST(PhaseLogHelpers, AssignBaseAndRenumber) {
    std::unordered_map<std::string, std::pair<int,int>> imported;
    auto a = canonicalPath("/tmp/a.bas");
    auto b = canonicalPath("/tmp/b.bas");
    imported[a] = {0, 10};
    int baseSame = assignBase(a, a, imported);
    int baseOther = assignBase(b, a, imported);
    ASSERT_EQ(baseSame, 0);
    ASSERT_GT(baseOther, 0);
    Program p{}; p.lines.push_back(Line{100,{}});
    int minImp=0; renumberProgram(p, baseOther, minImp);
    ASSERT_GT(p.lines.front().number, 100);
}
