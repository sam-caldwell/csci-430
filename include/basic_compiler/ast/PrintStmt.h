// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_PRINTSTMT_H
#define BASIC_COMPILER_AST_PRINTSTMT_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: PrintStmt
 * Purpose:
 *  - PRINT a string or numeric expression using printf.
 * Inputs:
 *  - value: Expression to print (StringExpr or numeric Expr)
 * Outputs:
 *  - Concrete Stmt node; codegen routes to @printf with proper format
 * Theory of operation:
 *  - String literals use "%s\n"; numeric expressions use "%f\n".
 */
struct PrintStmt : ASTLeaf<NodeKind::PrintStmt, Stmt> {
    // Separator between successive items
    enum class Sep : std::uint8_t { Comma, Semicolon };
    // Trailing terminator at end of PRINT
    enum class Terminator : std::uint8_t { Newline, Semicolon, Comma };
    // Backward-compatible single value; additional items in 'more'
    std::unique_ptr<Expr> value;
    std::vector<std::unique_ptr<Expr>> more;
    // Separators between items (size = total_items - 1)
    std::vector<Sep> seps;
    // Trailing terminator (default newline)
    Terminator trail{Terminator::Newline};
    // Optional: channel (PRINT #n, ...). -1 means stdout
    int channel{-1};
    // Optional: format expression from PRINT USING ... (string expr)
    std::unique_ptr<Expr> format;

    explicit PrintStmt(std::unique_ptr<Expr> single_value)
        : value(std::move(single_value)) {}
    explicit PrintStmt(std::vector<std::unique_ptr<Expr>> values) {
        if (!values.empty()) {
            value = std::move(values.front());
            more.reserve(values.size() - 1);
            for (std::size_t i = 1; i < values.size(); ++i) {
                more.emplace_back(std::move(values[i]));
            }
        }
    }
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_PRINTSTMT_H
