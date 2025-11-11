// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Program.h"
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
void CodeGenerator::cdCollectDeleteAndOptions(const Program& program, // NOLINT(readability-function-size)
                                              bool& printZones,
                                              std::vector<std::pair<int,int>>& deleteRanges,
                                              const int globalMin,
                                              const int globalMax) {
    deleteRanges.clear();
    for (const auto&[number, statements] : program.lines) {
        for (const auto& stmtNode : statements) {
            if (tryHandleOptionPrintZones(stmtNode.get(), printZones)) { continue; }
            if (tryHandleDelete(stmtNode.get(), number, globalMin, globalMax, deleteRanges)) { continue; }
        }
    }
}

} // namespace gwbasic
