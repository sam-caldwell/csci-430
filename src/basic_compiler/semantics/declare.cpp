// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <string>

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::declare
 * Inputs:
 *  - name: Variable identifier to declare
 * Outputs:
 *  - void (adds symbol to global scope and vars_ set)
 * Theory of operation:
 *  - Ensures a global scope exists, inserts 'name' into it, and records the
 *    variable in the global variables set; logs the declaration.
 */
void SemanticAnalyzer::declare(const std::string& name) {
    // BASIC variables are global; record declarations in the global scope
    if (scopes_.empty()) { scopes_.emplace_back(); }
    scopes_.front().insert(name);
    vars_.insert(name);
    
}

} // namespace gwbasic
