// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <optional>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: RunStmt
 * Purpose:
 *  - Restart program execution (clear variables and branch to start or a
 *    specified line number), modeling GW-BASIC RUN.
 * Inputs:
 *  - targetLine: Optional line to start execution from after reset.
 * Outputs:
 *  - Concrete Stmt node; codegen emits stores of 0.0 to all variables and
 *    an unconditional branch to the chosen entry line.
 */
struct RunStmt : ASTLeaf<NodeKind::RunStmt, Stmt> {
    std::optional<std::string> filename;
    std::optional<int> targetLine;
    RunStmt() = default;
    RunStmt(std::optional<std::string> f, std::optional<int> ln)
        : ASTLeaf(), filename(std::move(f)), targetLine(ln) {}
};

} // namespace gwbasic
