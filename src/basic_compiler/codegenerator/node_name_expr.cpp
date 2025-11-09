// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/RTTI.h"

namespace gwbasic {

/*
 * Function: nodeName
 * Summary: Pretty-print the AST kind for an expression node.
 * Parameters:
 *  - e: Expression pointer (nullable).
 * Returns:
 *  - const char*: Human-readable node kind name.
 */
const char* CodeGenerator::nodeName(const Expr* e) {
    return prettyName(e ? e->getKind() : NodeKind::AbstractExpr);
}

} // namespace gwbasic
