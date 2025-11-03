// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

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
    explicit MergeStmt(std::string f) : ASTLeaf(), filename(std::move(f)) {}
};

} // namespace gwbasic

