// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: InputStmt
 * Purpose:
 *  - Read a numeric value from stdin and assign to a variable.
 * Inputs:
 *  - name: Variable identifier to store into
 * Outputs:
 *  - Concrete Stmt node; codegen emits scanf-like logic (or stub)
 * Theory of operation:
 *  - Current implementation may be simplified; semantics logged for tracing.
 */
struct InputStmt : ASTLeaf<NodeKind::InputStmt, Stmt> {
    std::string name;
    explicit InputStmt(std::string n) : ASTLeaf(), name(std::move(n)) {}
};

} // namespace gwbasic
