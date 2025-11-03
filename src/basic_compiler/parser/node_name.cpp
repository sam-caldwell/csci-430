// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

/*
 * Function: Parser::nodeName
 * Inputs:
 *  - s: Statement pointer (may be null)
 * Outputs:
 *  - const char*: Human-readable type name for diagnostics
 * Theory of operation:
 *  - Delegates to prettyName() using the statement's NodeKind, falling back
 *    to AbstractStmt for null pointers.
 */
const char* Parser::nodeName(const Stmt* s) {
    return prettyName(s ? s->getKind() : NodeKind::AbstractStmt);
}

} // namespace gwbasic
