// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
Test: Integration.Metrics_TableFormatting
Inputs: A populated Metrics object
Code under test: printMetricsTable
Expected behavior: Produces an ASCII table with expected section headers and rows
*/
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

TEST(Integration, Metrics_TableFormatting) {
    Metrics m;
    // Fill minimal but varied values
    m.lexer.token_count = 12;
    m.ast_parsed = {2, 3, 2, 2, 1.5};
    m.ast_after_semantics = {2, 3, 2, 2, 1.5};
    m.ast_after_opt = {2, 3, 1, 1, 1.0};
    m.semantics_opt.const_folds = 2;
    m.semantics_opt.fold_add = 1;
    m.semantics_opt.fold_mul = 1;
    m.semantics_opt.unary_elim_plus = 1;
    m.semantics_opt.id_add_zero = 1;
    m.codegen.ir_instructions = 42;
    m.codegen.opt_phase_ir_counts = {{"-O1", 40}, {"-O2", 38}};

    std::ostringstream oss;
    printMetricsTable(m, oss);
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

