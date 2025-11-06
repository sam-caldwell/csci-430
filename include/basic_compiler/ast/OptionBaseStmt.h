// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: OptionBaseStmt
 * Purpose:
 *  - OPTION BASE 0|1 directive affecting array lower bounds.
 */
struct OptionBaseStmt : ASTLeaf<NodeKind::OptionBaseStmt, Stmt> {
    int base{0}; // 0 or 1
    OptionBaseStmt() = default;
    explicit OptionBaseStmt(int b) : ASTLeaf(), base(b) {}
};

} // namespace gwbasic

