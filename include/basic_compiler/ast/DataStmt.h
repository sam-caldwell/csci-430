// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_DATASTMT_H
#define BASIC_COMPILER_AST_DATASTMT_H

#include <utility>
#include <vector>

#include "basic_compiler/ast/DataItem.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: DataStmt
 * Purpose:
 *  - Holds compile-time DATA list items preserving quoted-string vs numeric
 *    tokenization and their normalized text payload in source order.
 */
struct DataStmt : ASTLeaf<NodeKind::DataStmt, Stmt> {
    std::vector<DataItem> items;
    DataStmt() = default;
    explicit DataStmt(std::vector<DataItem> items_vec) : items(std::move(items_vec)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_DATASTMT_H
