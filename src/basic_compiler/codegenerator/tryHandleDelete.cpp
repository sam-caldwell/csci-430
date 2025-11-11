#include "basic_compiler/ast/DeleteStmt.h"
#include "basic_compiler/ast/OptionPrintZonesStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {
    /***
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
    bool tryHandleDelete(const Stmt* stmt,
                                int lineNumber,
                                int globalMin,
                                int globalMax, // NOLINT(bugprone-easily-swappable-parameters)
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
} // namespace gwbasic::tryHandleDelete
