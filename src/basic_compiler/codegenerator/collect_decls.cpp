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
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"
#include "basic_compiler/ast/ResumeStmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/OptionPrintZonesStmt.h"
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
            for (const auto& st : line.statements)
                collectStmtVars(st.get());
        }
        // Always scan for RND/STOP usage to decide helper/global emission
        for (const auto& st : line.statements) {
            scanStmtForRnd(st.get());
            scanStmtForStop(st.get());
            // Also, pick up OPTION PRINTZONES directives directly to drive
            // comma-zone padding even if semantics were not provided.
            if (const auto* opz = dyn_cast<const OptionPrintZonesStmt>(st.get())) {
                printZones_ = opz->enabled;
            }
        }
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
                    for (const auto& n : cs->names)
                        accumCommon.insert(n);
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

    // Regardless of semantics, ensure prompt literals in INPUT are assigned ids
    for (const auto&[number, statements] : program.lines) {
        for (const auto& st : statements) {
            if (const auto* in = dyn_cast<const InputStmt>(st.get())) {
                if (in->promptLiteral && !strLiteralId_.contains(*in->promptLiteral)) {
                    strLiteralId_[*in->promptLiteral] = strCounter_++;
                }
            }
        }
    }

    // Populate DATA items into dataLiteralIds_ and ensure each item has an id
    dataLiteralIds_.clear();
    dataIsString_.clear();
    dataNumValues_.clear();
    for (int ln : lineNumbers_) {
        const auto* lptr = lineMap_[ln];
        if (!lptr) continue;
        for (const auto& st : lptr->statements) {
            if (const auto ds = dyn_cast<const DataStmt>(st.get())) {
                for (const auto& it : ds->items) {
                    const std::string& txt = it.text;
                    if (!strLiteralId_.contains(txt)) strLiteralId_[txt] = strCounter_++;
                    dataLiteralIds_.push_back(strLiteralId_[txt]);
                    dataIsString_.push_back(it.isString ? 1u : 0u);
                    if (!it.isString) {
                        try {
                            dataNumValues_.push_back(std::stod(txt));
                        } catch (...) {
                            // Fallback: treat invalid numeric text as 0.0; parser should prevent this.
                            dataNumValues_.push_back(0.0);
                        }
                    } else {
                        dataNumValues_.push_back(0.0);
                    }
                }
            }
        }
    }

    // Build mapping of DATA index at the start of each 1000-based line region
    // independent of whether semantics were provided.
    regionDataStartIdx_.clear();
    int dataCount = 0;
    for (int ln : lineNumbers_) {
        const int region = (ln / 1000) * 1000;
        if (!regionDataStartIdx_.contains(region)) {
            regionDataStartIdx_[region] = dataCount; // snapshot at first line in region
        }
        const auto* lptr = lineMap_[ln];
        if (!lptr) continue;
        for (const auto& st : lptr->statements) {
            if (const auto ds = dyn_cast<const DataStmt>(st.get())) {
                dataCount += static_cast<int>(ds->items.size());
            }
        }
    }

    // Compute handler skip destinations: for each ON ERROR GOTO target line T,
    // find the first subsequent line that contains a RESUME and set skip to
    // the line following it (or exit if none), so normal fallthrough skips
    // handler blocks when not in handler context.
    handlerSkipAfter_.clear();
    {
        std::set<int> trapTargets;
        for (const auto& [ln, lptr] : lineMap_) {
            (void)ln;
            if (!lptr) continue;
            for (const auto& st : lptr->statements) {
                if (const auto oeg = dyn_cast<const OnErrorGotoStmt>(st.get())) {
                    if (oeg->targetLine > 0)
                        trapTargets.insert(oeg->targetLine);
                }
            }
        }
        // For each trap start, find the first line with RESUME from that start
        for (int t : trapTargets) {
            // locate index of t
            int startIdx = -1;
            for (size_t i = 0; i < lineNumbers_.size(); ++i) {
                if (lineNumbers_[i] == t) {
                    startIdx = static_cast<int>(i);
                    break;
                }
            }
            if (startIdx < 0) continue;
            int endIdx = -1;
            for (int j = startIdx; j < static_cast<int>(lineNumbers_.size()); ++j) {
                const auto* lp = lineMap_[lineNumbers_[j]];
                if (!lp) continue;
                bool hasResume = false;
                for (const auto& st : lp->statements) {
                    if (isa<const ResumeStmt>(st.get())) {
                        hasResume = true;
                        break;
                    }
                }
                if (hasResume) { endIdx = j; break; }
            }
            int skipTo = -1;
            if (endIdx >= 0) {
                if (endIdx + 1 < static_cast<int>(lineNumbers_.size()))
                    skipTo = lineNumbers_[endIdx + 1];
            } else {
                if (startIdx + 1 < static_cast<int>(lineNumbers_.size()))
                    skipTo = lineNumbers_[startIdx + 1];
            }
            handlerSkipAfter_[t] = skipTo; // -1 means exit
        }
    }
}

} // namespace gwbasic
