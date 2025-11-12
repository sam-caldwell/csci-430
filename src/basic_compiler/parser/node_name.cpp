// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/Traits.h"

namespace gwbasic {

/*
 * Function: Parser::nodeName
 * Summary:
 *  Return a human-readable statement type name for diagnostics.
 * Parameters:
 *  - stmt: Statement pointer (may be null)
 * Returns:
 *  - const char*: Pretty name for the node kind
 */
const char* Parser::nodeName(const Stmt* stmt) {
    return prettyName((stmt != nullptr) ? stmt->getKind() : NodeKind::AbstractStmt);
}

} // namespace gwbasic
