// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DeleteStmt.h"
#include "basic_compiler/ast/OptionPrintZonesStmt.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: cdCollectDeleteAndOptions
 * Summary: Scan statements to capture DELETE ranges and OPTION PRINT ZONES.
 * Parameters:
 *  - program: Program AST to scan.
 *  - printZones: Output flag set if OPTION PRINT ZONES is enabled.
 *  - deleteRanges: Output DELETE ranges collected (may be open-ended initially).
 *  - globalMin/globalMax: Known bounds to resolve open-ended DELETEs.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
namespace {
/*
 * Function: tryHandleOptionPrintZones
 * Summary: Detect and apply OPTION PRINT ZONES statement.
 * Parameters:
 *  - stmt: Candidate statement pointer.
 *  - printZones: Output flag set when enabled by statement.
 * Returns:
 *  - bool: true if stmt was OPTION PRINT ZONES and handled; else false.
 */
inline bool tryHandleOptionPrintZones(const Stmt* stmt, bool& printZones) {
    if (const auto* const opz = dyn_cast<const OptionPrintZonesStmt>(stmt)) {
        printZones = opz->enabled;
        return true;
    }
    return false;
}
/*
 * Function: tryHandleDelete
 * Summary: Detect DELETE statement and append its normalized range.
 * Parameters:
 *  - stmt: Candidate statement pointer.
 *  - lineNumber: Current line number (for '.' shorthand resolution).
 *  - globalMin/globalMax: Known program bounds.
 *  - deleteRanges: Output ranges appended when detected.
 * Returns:
 *  - bool: true if stmt was DELETE and handled; else false.
 */
inline bool tryHandleDelete(const Stmt* stmt, int lineNumber, int globalMin, int globalMax, // NOLINT(bugprone-easily-swappable-parameters)
                            std::vector<std::pair<int,int>>& deleteRanges) {
    if (const auto* const del = dyn_cast<const DeleteStmt>(stmt)) {
        int start = del->startLine.has_value() ? *del->startLine : globalMin;
        int end   = del->endLine.has_value()   ? *del->endLine   : globalMax;
        if (del->startIsDot) { start = lineNumber; }
        if (del->endIsDot)   { end   = lineNumber; }
        deleteRanges.emplace_back(start, end);
        return true;
    }
    return false;
}
} // namespace

/*
 * Function: cdCollectDeleteAndOptions
 * Summary: Scan statements to capture DELETE ranges and OPTION PRINT ZONES.
 * Parameters:
 *  - program: Program AST to scan.
 *  - printZones: Output flag set if OPTION PRINT ZONES is enabled.
 *  - deleteRanges: Output DELETE ranges collected (may be open-ended initially).
 *  - globalMin/globalMax: Known bounds to resolve open-ended DELETEs.
 * Returns:
 *  - void
 */
void CodeGenerator::cdCollectDeleteAndOptions(const Program& program, // NOLINT(readability-function-size)
                                              bool& printZones,
                                              std::vector<std::pair<int,int>>& deleteRanges,
                                              int globalMin,
                                              int globalMax) {
    deleteRanges.clear();
    for (const auto& line : program.lines) {
        for (const auto& stmtNode : line.statements) {
            if (tryHandleOptionPrintZones(stmtNode.get(), printZones)) { continue; }
            if (tryHandleDelete(stmtNode.get(), line.number, globalMin, globalMax, deleteRanges)) { continue; }
        }
    }
}

} // namespace gwbasic
