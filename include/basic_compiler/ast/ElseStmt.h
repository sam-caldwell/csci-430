// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

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
