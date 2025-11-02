// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <optional>
#include <string>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: NextStmt
 * Purpose:
 *  - Marker statement delimiting the end of a FOR block in multi-line form.
 * Inputs:
 *  - var: Optional induction variable name (used for matching/clarity only).
 * Outputs:
 *  - Concrete Stmt recognized by the parser; eliminated during parseProgram
 *    restructuring when FOR bodies are folded.
 */
struct NextStmt : ASTLeaf<NodeKind::NextStmt, Stmt> {
    std::optional<std::string> var;
    NextStmt() = default;
    explicit NextStmt(std::optional<std::string> v) : ASTLeaf(), var(std::move(v)) {}
};

} // namespace gwbasic

