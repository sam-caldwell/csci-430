// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <algorithm>
// AST headers needed for per-line variable/array collection
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/DimStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"

namespace gwbasic {

/*
 * Function: CodeGenerator::collectDecls
 * Inputs:
 *  - program: AST to be compiled
 * Outputs:
 *  - void (initializes internal maps/sets and prepares line ordering)
 * Theory of operation:
 *  - Clear internal state,
 *  - Scan all lines/statements to populate the sets of variables and string literals,
 *  - Records and sorts line numbers and
 *  - builds a line-number to Line* map for later codegen.
 */
void CodeGenerator::collectDecls(const Program& program) {
    variables_.clear();
    commonVariables_.clear();
    varAllocaName_.clear();
    strLiteralId_.clear();
    tempCounter_ = 0;
    strCounter_ = 0;
    lineNumbers_.clear();
    lineMap_.clear();
    needsRndHelper_ = false;
    commonBeforeLine_.clear();
    varsBeforeLine_.clear();
    arraysBeforeLine_.clear();

    for (const auto& line : program.lines) {
        lineNumbers_.push_back(line.number);
        lineMap_[line.number] = &line;
        if (!semProvided_) {
            for (const auto& st : line.statements) collectStmtVars(st.get());
        }
        // Always scan for RND usage to decide helper emission
        for (const auto& st : line.statements) scanStmtForRnd(st.get());
    }
    std::ranges::sort(lineNumbers_);
    lineNumbers_.erase(std::ranges::unique(lineNumbers_).begin(), lineNumbers_.end());

    // Build mapping of COMMON variables that are in effect before each line
    {
        // Track COMMON variables seen before each line
        std::set<std::string> accumCommon;
        // Track variables/arrays seen before each line (by name)
        std::set<std::string> accumVars;
        std::set<std::string> accumArrays;
        for (int ln : lineNumbers_) {
            // Record snapshots before processing this line
            commonBeforeLine_[ln] = accumCommon;
            varsBeforeLine_[ln] = accumVars;
            arraysBeforeLine_[ln] = accumArrays;
            const auto* lptr = lineMap_[ln];
            if (!lptr) continue;
            // Update accumulators based on statements in this line
            for (const auto& st : lptr->statements) {
                if (const auto cs = dyn_cast<const CommonStmt>(st.get())) {
                    for (const auto& n : cs->names) accumCommon.insert(n);
                }
                collectVarsForBeforeLineFromStmt(st.get(), accumVars, accumArrays);
            }
        }
    }

    if (semProvided_) {
        // Seed variables and strings from semantics
        variables_ = semVariables_;
        commonVariables_ = semCommonVariables_;
        strLiteralId_.clear();
        strCounter_ = 0;
        for (const auto& s : semStrings_) {
            // Only assign ids once; stable deterministic order
            if (!strLiteralId_.contains(s)) strLiteralId_[s] = strCounter_++;
        }
        // LineNumbers are computed from AST to drive emission order; no change
    }
}

// helpers moved to separate compilation units to satisfy one-function-per-file rule

} // namespace gwbasic
