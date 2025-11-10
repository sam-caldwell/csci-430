// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_OPENSTMT_H
#define BASIC_COMPILER_AST_OPENSTMT_H

#include <cstdint>
#include <memory>
#include <utility>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

enum class FileMode : std::uint8_t { Input, Output };

/**
 * Type: OpenStmt
 * Purpose:
 *  - OPEN <string> FOR (INPUT|OUTPUT) AS #<n> — open a file channel.
 * Inputs:
 *  - filename: String expression naming the file
 *  - mode: FileMode (Input or Output)
 *  - channel: Logical channel number (1..16)
 * Outputs:
 *  - Concrete Stmt node; semantics validate range and types
 */
struct OpenStmt final : ASTLeaf<NodeKind::OpenStmt, Stmt> {
    std::unique_ptr<Expr> filename;
    FileMode mode{FileMode::Input};
    int channel{1};
    OpenStmt() = default;
    OpenStmt(std::unique_ptr<Expr> filename_expr, const FileMode file_mode, const int channel_num)
        : filename(std::move(filename_expr)), mode(file_mode), channel(channel_num) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_OPENSTMT_H
