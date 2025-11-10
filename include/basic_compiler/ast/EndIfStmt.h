// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ENDIFSTMT_H
#define BASIC_COMPILER_AST_ENDIFSTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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

#endif // BASIC_COMPILER_AST_ENDIFSTMT_H
