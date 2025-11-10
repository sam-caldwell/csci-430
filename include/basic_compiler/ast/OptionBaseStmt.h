// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_OPTIONBASESTMT_H
#define BASIC_COMPILER_AST_OPTIONBASESTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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

#endif // BASIC_COMPILER_AST_OPTIONBASESTMT_H
