// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: EndIfStmt (marker)
 * Purpose:
 *  - Structural marker emitted by the parser to delimit END IF in a block
 *    before restructuring is performed in parseProgram().
 * Inputs: none
 * Outputs:
 *  - Temporary Stmt eliminated during block folding
 */
struct EndIfStmt : ASTLeaf<NodeKind::EndIfStmt, Stmt> {
    EndIfStmt() = default;
};

} // namespace gwbasic
