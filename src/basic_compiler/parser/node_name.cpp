// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

/*
 * Function: Parser::nodeName
 * Summary:
 *  Return a human-readable statement type name for diagnostics.
 * Parameters:
 *  - s: Statement pointer (may be null)
 * Returns:
 *  - const char*: Pretty name for the node kind
 */
const char* Parser::nodeName(const Stmt* s) {
    return prettyName(s ? s->getKind() : NodeKind::AbstractStmt);
}

} // namespace gwbasic
