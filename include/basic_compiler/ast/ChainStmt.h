// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_CHAINSTMT_H
#define BASIC_COMPILER_AST_CHAINSTMT_H

#include <optional>
#include <string>

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: ChainStmt
 * Purpose:
 *  - Transfer control to another program (or line), preserving COMMON
 *    variables by default, or ALL variables when specified.
 * Inputs:
 *  - filename: optional program name
 *  - targetLine: optional line to begin execution
 *  - all: if true, preserve all variables; otherwise, only COMMON ones.
 * Outputs:
 *  - Concrete Stmt node; codegen resets non-preserved variables to 0.0 and
 *    branches to the chosen entry.
 */
struct ChainStmt : ASTLeaf<NodeKind::ChainStmt, Stmt> {
    std::optional<std::string> filename;
    std::optional<int> targetLine;
    bool all{false};
    ChainStmt() = default;
    ChainStmt(std::optional<std::string> f, const std::optional<int> ln, const bool a)
        : ASTLeaf(), filename(std::move(f)), targetLine(ln), all(a) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_CHAINSTMT_H
