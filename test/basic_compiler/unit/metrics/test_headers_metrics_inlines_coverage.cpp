// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>

#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

/***
 * Test: Headers.Metrics_Inlines_Coverage
 * Purpose: Exercise inline helpers and counters to improve header coverage for Metrics.
 * Components Under Test: Metrics inline methods and counters.
 * Expected Behavior: No crash; printed table contains expected sections.
 */
TEST(Headers, Metrics_Inlines_Coverage) {
    Metrics m;
    // Analysis-only flag
    EXPECT_FALSE(m.isAnalyzeOnly());
    m.setAnalyzeOnly(true);
    EXPECT_TRUE(m.isAnalyzeOnly());
    m.setAnalyzeOnly(false);
    EXPECT_FALSE(m.isAnalyzeOnly());

    // Lexer
    m.incToken();

    // Semantics/optimizer counters
    m.incConstFoldAdd();
    m.incConstFoldSub();
    m.incConstFoldMul();
    m.incConstFoldDiv();
    m.incConstFoldCmp();
    m.incUnaryElimPlus();
    m.incUnaryConstMinus();
    m.incIdAddZero();
    m.incIdSubZero();
    m.incIdMulOne();
    m.incIdMulZero();
    m.incIdDivOne();
    m.incIfConstTrueToGoto();
    m.incIfConstFalseRemoved();
    m.incForStepElided();

    // Codegen counters setters
    m.setIrInstructionCount(123);
    m.setOptPhaseCounts({{"-O1", 10}, {"-O2", 9}});

    // Print to a stream (indirectly exercises header-controlled state)
    std::ostringstream os;
    m.print(os);
    auto out = os.str();
    ASSERT_NE(out.find("Lexer"), std::string::npos);
    ASSERT_NE(out.find("Codegen"), std::string::npos);
}
