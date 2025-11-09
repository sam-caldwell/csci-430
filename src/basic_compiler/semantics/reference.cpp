// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/SourcePos.h"
#include <string>

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
    // Inside DEF FN body, treat the parameter as local (do not record as global var)
    if (currentFnParam_.has_value() && *currentFnParam_ == name) {
        log() << "FnParamRef " << name << " @ " << pos.line << ':' << pos.col << '\n';
        return;
    }
    if (!isDeclared(name)) {
        declare(name);
        log() << "VarImplicitDecl " << name << " @ " << pos.line << ':' << pos.col << '\n';
    } else {
        log() << "VarRef " << name << " @ " << pos.line << ':' << pos.col << '\n';
    }
}

} // namespace gwbasic
