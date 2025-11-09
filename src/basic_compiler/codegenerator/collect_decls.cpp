// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <algorithm>
// AST headers needed for per-line variable/array collection
#include "basic_compiler/ast/DimStmt.h"


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

    int gmin = 0;
    int gmax = 0;
    cdGatherLinesAndDeletes(
        program, lineNumbers_, lineMap_, printZones_,
        delRanges, gmin, gmax
    );

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
