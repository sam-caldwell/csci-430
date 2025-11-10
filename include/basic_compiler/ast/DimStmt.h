// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_DIMSTMT_H
#define BASIC_COMPILER_AST_DIMSTMT_H

#include <string>
#include <vector>

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: DimStmt
 * Purpose:
 *  - Declare one array with one or more dimensions: DIM A(10[,m...])
 */
struct DimStmt : ASTLeaf<NodeKind::DimStmt, Stmt> {
    std::string name;
    // Per-dimension declared upper bounds (inclusive), e.g., DIM A(10,20) -> {10,20}
    std::vector<int> upperBounds;
    DimStmt() = default;
    DimStmt(std::string n, std::vector<int> ub) : ASTLeaf(), name(std::move(n)), upperBounds(std::move(ub)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_DIMSTMT_H
