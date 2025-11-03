// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <vector>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: CommonStmt
 * Purpose:
 *  - Declare variables to be placed in COMMON storage (for CHAIN
 *    compatibility). In this compiler, COMMON acts as a declaration that the
 *    listed variables exist and are global.
 * Inputs:
 *  - names: list of identifiers
 * Outputs:
 *  - Concrete Stmt with no direct codegen effect.
 */
struct CommonStmt : ASTLeaf<NodeKind::CommonStmt, Stmt> {
    std::vector<std::string> names;
    CommonStmt() = default;
    explicit CommonStmt(std::vector<std::string> ns) : ASTLeaf(), names(std::move(ns)) {}
};

} // namespace gwbasic

