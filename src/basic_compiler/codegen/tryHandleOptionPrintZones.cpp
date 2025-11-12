#include "basic_compiler/ast/DeleteStmt.h"
#include "basic_compiler/ast/OptionPrintZonesStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {
    /***
     * Function: tryHandleOptionPrintZones
     * Summary: Detect and apply OPTION PRINT ZONES statement.
     * Parameters:
     *  - stmt: Candidate statement pointer.
     *  - printZones: Output flag set when enabled by statement.
     * Returns:
     *  - bool: true if stmt was OPTION PRINT ZONES and handled; else false.
     */
    bool tryHandleOptionPrintZones(const Stmt* stmt, bool& printZones) {
        if (const auto* const opz = dyn_cast<const OptionPrintZonesStmt>(stmt)) {
            printZones = opz->enabled;
            return true;
        }
        return false;
    }
} //namespace gwbasic
