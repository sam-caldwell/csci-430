// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::reference
 * Inputs:
 *  - name: Variable identifier being referenced
 *  - pos: Source position of the reference (for logging)
 * Outputs:
 *  - void (declares implicitly if needed and logs the reference)
 * Theory of operation:
 *  - If the variable is undeclared, declares it in the global scope and
 *    logs implicit declaration; otherwise logs a reference event.
 */
void SemanticAnalyzer::reference(const std::string& name, const SourcePos& pos) {
    if (!isDeclared(name)) {
        declare(name);
        std::ostringstream m; m << "VarImplicitDecl " << name << " @ " << pos.line << ':' << pos.col; log(m.str());
    } else {
        std::ostringstream m; m << "VarRef " << name << " @ " << pos.line << ':' << pos.col; log(m.str());
    }
}

} // namespace gwbasic
