// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <vector>
#include <optional>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: InputStmt
 * Purpose:
 *  - Read values from stdin and assign to one or more variables.
 * Inputs:
 *  - variables: One or more destination variable identifiers (numeric only in this compiler).
 *  - promptLiteral: Optional literal string to print before reading.
 *  - promptVar: Optional string variable name to print before reading.
 * Outputs:
 *  - Concrete Stmt node; codegen emits scanf-like logic (or stub)
 * Theory of operation:
 *  - Current implementation supports numeric variable targets only; use LINE INPUT for strings.
 */
struct InputStmt : ASTLeaf<NodeKind::InputStmt, Stmt> {
    std::vector<std::string> variables;
    std::optional<std::string> promptLiteral; // when INPUT "text"; var[,...]
    std::optional<std::string> promptVar;     // when INPUT ; P$, var[,...]

    InputStmt() = default;
    explicit InputStmt(std::vector<std::string> names)
        : ASTLeaf(), variables(std::move(names)) {}
};

} // namespace gwbasic
