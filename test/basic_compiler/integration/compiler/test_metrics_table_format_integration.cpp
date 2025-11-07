// (c) 2025 Sam Caldwell. All Rights Reserved.
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

/***
 * Test: Integration.Metrics_TableFormatting
 * Purpose: Produce a metrics table from a populated Metrics object and verify expected sections/rows.
 * Components Under Test: printMetricsTable (via Metrics::print).
 * Expected Behavior: ASCII table contains key section headers and some sample numeric values.
 */
TEST(Integration, Metrics_TableFormatting) {
    Metrics m;
    // Fill minimal but varied values
    MetricsAccessorForTests::setLexerTokenCount(m, 12);
    MetricsAccessorForTests::setAstParsed(m, 2, 3, 2, 2, 1.5);
    MetricsAccessorForTests::setAstAfterSemantics(m, 2, 3, 2, 2, 1.5);
    MetricsAccessorForTests::setAstOptimized(m, 2, 3, 1, 1, 1.0);
    MetricsAccessorForTests::setSemanticsBasic(m, 2, 1, 1, 1, 1);
    MetricsAccessorForTests::setIrInstructions(m, 42);
    MetricsAccessorForTests::setOptPhaseCounts(m, {{"-O1", 40}, {"-O2", 38}});

    std::ostringstream oss;
    m.print(oss);
    const std::string tbl = oss.str();

    // Basic structure
    ASSERT_NE(tbl.find("+"), std::string::npos);
    ASSERT_NE(tbl.find("| Metric"), std::string::npos);
    ASSERT_NE(tbl.find("| Phase"), std::string::npos);

    // Key rows
    ASSERT_NE(tbl.find("Lexer"), std::string::npos);
    ASSERT_NE(tbl.find("AST (parsed)"), std::string::npos);
    ASSERT_NE(tbl.find("Semantics Opt"), std::string::npos);
    ASSERT_NE(tbl.find("Codegen"), std::string::npos);
    ASSERT_NE(tbl.find("IR instructions"), std::string::npos);

    // Values present
    ASSERT_NE(tbl.find("12"), std::string::npos);
    ASSERT_NE(tbl.find("42"), std::string::npos);
}
