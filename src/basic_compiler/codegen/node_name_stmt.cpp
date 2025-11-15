// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Traits.h"

namespace gwbasic {

/*
 * Function: nodeName
 * Summary: Pretty-print the AST kind for a statement node.
 * Parameters:
 *  - s: Statement pointer (nullable).
 * Returns:
 *  - const char*: Human-readable node kind name.
 */
const char* CodeGenerator::nodeName(const Stmt* stmt) {
    return prettyName((stmt != nullptr) ? stmt->getKind() : NodeKind::AbstractStmt);
}

} // namespace gwbasic
