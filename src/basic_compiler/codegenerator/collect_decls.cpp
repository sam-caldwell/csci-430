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
#include "basic_compiler/ast/DeleteStmt.h"
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
    variables_.clear(); commonVariables_.clear(); varAllocaName_.clear(); strLiteralId_.clear();
    tempCounter_ = 0; strCounter_ = 0; lineNumbers_.clear(); lineMap_.clear(); needsRndHelper_ = false;
    commonBeforeLine_.clear(); varsBeforeLine_.clear(); arraysBeforeLine_.clear();

    // Phase A: gather and filter lines
    std::vector<std::pair<int,int>> delRanges;
    int gmin = 0, gmax = 0;
    cdGatherLinesAndDeletes(program, lineNumbers_, lineMap_, printZones_, delRanges, gmin, gmax);
    if (lineNumbers_.empty()) return;
    cdFilterDeletedLines(lineNumbers_, delRanges);

    // Phase B: per-line scans and snapshots
    cdCollectVarsIfNoSemantics(lineNumbers_);
    cdScanRndAndStop(lineNumbers_);
    cdBuildBeforeLineSnapshots(lineNumbers_);

    // Phase C: seed from semantics + literals, data, regions, handlers
    cdSeedFromSemantics();
    cdAssignInputPromptLiteralIds(lineNumbers_);
    cdCollectDataItems(lineNumbers_);
    cdBuildRegionDataStartIdx(lineNumbers_);
    cdComputeHandlerSkipAfter(lineNumbers_);
}

} // namespace gwbasic
