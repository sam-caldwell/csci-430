// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/StringExpr.h"

using namespace gwbasic;
using namespace gwbasic::phase_log_helpers;

/***
 * Test: Phase.Merge_ReplaceAndAppend_Semantics
 * Purpose: Verify appendMergeProgramReplacing() replaces duplicate line numbers,
 * while replaceOrAppendLine(..., replace=false) appends (no replacement).
 */
TEST(Phase, Merge_ReplaceAndAppend_Semantics) {
    Program dst;
    // dst: 10 PRINT "A"
    {
        Line l; l.number = 10;
        auto ps = std::make_unique<PrintStmt>(std::make_unique<StringExpr>(std::string("A")));
        l.statements.emplace_back(std::move(ps));
        replaceOrAppendLine(dst, std::move(l), /*replace=*/false);
    }
    // src: 10 PRINT "B"
    Program src;
    {
        Line l; l.number = 10;
        auto ps = std::make_unique<PrintStmt>(std::make_unique<StringExpr>(std::string("B")));
        l.statements.emplace_back(std::move(ps));
        src.lines.emplace_back(std::move(l));
    }
    // MERGE: replaces duplicate line in dst with src’s line
    appendMergeProgramReplacing(dst, std::move(src));
    ASSERT_EQ(dst.lines.size(), 1u);
    // Verify content is now "B"
    auto* pr = gwbasic::dyn_cast<PrintStmt>(dst.lines[0].statements[0].get());
    ASSERT_NE(pr, nullptr);
    auto* se = gwbasic::dyn_cast<StringExpr>(pr->value.get());
    ASSERT_NE(se, nullptr);
    EXPECT_EQ(se->value, "B");

    // Now append dst: 10 PRINT "C" without replacement should append a duplicate line
    Line l2; l2.number = 10;
    l2.statements.emplace_back(std::make_unique<PrintStmt>(std::make_unique<StringExpr>(std::string("C"))));
    replaceOrAppendLine(dst, std::move(l2), /*replace=*/false);
    ASSERT_EQ(dst.lines.size(), 2u);
}
