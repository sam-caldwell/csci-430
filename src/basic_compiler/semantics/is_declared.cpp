// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::isDeclared
 * Inputs:
 *  - name: Variable identifier
 * Outputs:
 *  - bool: true if 'name' exists in any active scope (innermost first)
 * Theory of operation:
 *  - Iterates scopes from innermost to outermost looking for the symbol.
 */
bool SemanticAnalyzer::isDeclared(const std::string& name) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        if (it->contains(name)) return true;
    }
    return false;
}

} // namespace gwbasic
