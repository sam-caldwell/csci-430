// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_MERGESTMT_H
#define BASIC_COMPILER_AST_MERGESTMT_H

#include <string>
#include <utility>

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: MergeStmt
 * Purpose:
 *  - Compile-time directive to merge another program's lines into this one.
 * Inputs:
 *  - filename: path to a .bas program
 * Outputs:
 *  - No direct codegen; parser resolves by replacing/inserting lines.
 */
struct MergeStmt : ASTLeaf<NodeKind::MergeStmt, Stmt> {
    std::string filename;
    explicit MergeStmt(std::string filename_str) : filename(std::move(filename_str)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_MERGESTMT_H
