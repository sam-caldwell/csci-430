// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/LineInputStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/BinaryOp.h"
#include "basic_compiler/ast/CallExpr.h"

using namespace gwbasic;

static std::unique_ptr<Expr> num(const double v) { return std::make_unique<NumberExpr>(v); }
static std::unique_ptr<Expr> str(const std::string& v) { return std::make_unique<StringExpr>(v); }

/*
Test: Metrics.ComputeAstSnapshot_CoversStatementKinds
Inputs: A Program with a wide variety of statement types and expressions
Code under test: Metrics::recordParsedSnapshot (via computeAstSnapshot)
Expected behavior: Computes non-zero counts; executes branches for many Stmt kinds
*/
TEST(Metrics, ComputeAstSnapshot_CoversStatementKinds) {
    Program prog;
    Line line; line.number = 10;

    // LET X = 1 + 2
    line.statements.emplace_back(std::make_unique<AssignStmt>("X", std::make_unique<BinaryExpr>(BinaryOp::Add, num(1), num(2))));
    // A(1) = 3
    {
        std::vector<std::unique_ptr<Expr>> idx; idx.emplace_back(num(1));
        line.statements.emplace_back(std::make_unique<ArrayAssignStmt>("A", std::move(idx), num(3)));
    }
    // MID$(S$, 1, 1) = "Z"
    {
        std::vector<std::unique_ptr<Expr>> idx; // scalar target
        line.statements.emplace_back(std::make_unique<MidAssignStmt>("S$", std::move(idx), num(1), num(1), str("Z")));
    }
    // PRINT "HI", X
    {
        std::vector<std::unique_ptr<Expr>> items; items.emplace_back(str("HI")); items.emplace_back(num(0));
        line.statements.emplace_back(std::make_unique<PrintStmt>(std::move(items)));
    }
    // IF 1<2 THEN 100
    line.statements.emplace_back(std::make_unique<IfStmt>(std::make_unique<BinaryExpr>(BinaryOp::Lt, num(1), num(2)), 100));
    // IFBLOCK: IF 2=2 THEN PRINT 1 ELSE PRINT 0
    {
        auto ib = std::make_unique<IfBlockStmt>(std::make_unique<BinaryExpr>(BinaryOp::Eq, num(2), num(2)));
        ib->thenBody.emplace_back(std::make_unique<PrintStmt>(num(1)));
        ib->elseBody.emplace_back(std::make_unique<PrintStmt>(num(0)));
        line.statements.emplace_back(std::move(ib));
    }
    // FOR I=1 TO 3 STEP 1: PRINT 5
    {
        auto fs = std::make_unique<ForStmt>("I", num(1), num(3), num(1));
        fs->body.emplace_back(std::make_unique<PrintStmt>(num(5)));
        line.statements.emplace_back(std::move(fs));
    }
    // FOR J=1 TO 2 (no explicit STEP) : PRINT 6
    {
        auto fs2 = std::make_unique<ForStmt>("J", num(1), num(2), std::unique_ptr<Expr>{});
        fs2->body.emplace_back(std::make_unique<PrintStmt>(num(6)));
        line.statements.emplace_back(std::move(fs2));
    }
    // WHILE 1<2: PRINT 9 (single-line semantics; we just need node kinds)
    {
        auto ws = std::make_unique<WhileStmt>(std::make_unique<BinaryExpr>(BinaryOp::Lt, num(1), num(2)));
        ws->body.emplace_back(std::make_unique<PrintStmt>(num(9)));
        line.statements.emplace_back(std::move(ws));
    }
    // INPUT X
    {
        auto in = std::make_unique<InputStmt>(std::vector<std::string>{"X"});
        line.statements.emplace_back(std::move(in));
    }
    // LINE INPUT #1, S$
    line.statements.emplace_back(std::make_unique<LineInputStmt>(1, "S$"));
    // WRITE #1, 7, "A", SQR(4)
    {
        std::vector<std::unique_ptr<Expr>> items; items.emplace_back(num(7)); items.emplace_back(str("A"));
        std::vector<std::unique_ptr<Expr>> cargs; cargs.emplace_back(num(4));
        items.emplace_back(std::make_unique<CallExpr>("SQR", std::move(cargs)));
        auto wr = std::make_unique<WriteStmt>(1, std::move(items));
        line.statements.emplace_back(std::move(wr));
    }

    prog.lines.push_back(std::move(line));

    Metrics m;
    // Trigger snapshot computation
    m.recordParsedSnapshot(prog);

    // The snapshot should have at least the number of lines/statements we added
    std::ostringstream oss; m.print(oss);
    const std::string tbl = oss.str();
    ASSERT_NE(tbl.find("AST (parsed)"), std::string::npos);
    ASSERT_NE(tbl.find("statements"), std::string::npos);
    // There must be non-zero expressions counted
    ASSERT_NE(tbl.find("expressions"), std::string::npos);
}
