// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_ERASESTMT_H
#define BASIC_COMPILER_AST_ERASESTMT_H

#include <string>
#include <vector>

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: EraseStmt
 * Purpose:
 *  - ERASE array[,array...] — release arrays and reset their contents.
 * Inputs:
 *  - names: list of array identifiers to erase
 */
struct EraseStmt final : ASTLeaf<NodeKind::EraseStmt, Stmt> {
    std::vector<std::string> names;
    EraseStmt() = default;
    explicit EraseStmt(std::vector<std::string> ns) : ASTLeaf(), names(std::move(ns)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_ERASESTMT_H
