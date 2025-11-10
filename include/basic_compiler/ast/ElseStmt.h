// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ELSESTMT_H
#define BASIC_COMPILER_AST_ELSESTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: ElseStmt (marker)
 * Purpose:
 *  - Structural marker emitted by the parser to delimit ELSE in an IF block
 *    before restructuring is performed in parseProgram().
 * Inputs: none
 * Outputs:
 *  - Temporary Stmt eliminated during block folding
 */
struct ElseStmt : ASTLeaf<NodeKind::ElseStmt, Stmt> {
    ElseStmt() = default;
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_ELSESTMT_H
