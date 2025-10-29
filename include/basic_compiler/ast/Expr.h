// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/SourcePos.h"

namespace gwbasic {

/**
 * Type: Expr (abstract)
 * Purpose:
 *  - Common base for all expression nodes (numeric, string, variable,
 *    unary/binary operations, etc.).
 * Inputs:
 *  - pos: Source position captured by the parser for diagnostics/logging
 * Outputs:
 *  - Virtual base enabling polymorphic handling of expressions
 * Theory of operation:
 *  - Concrete subclasses implement specific expression forms. Ownership is
 *    typically via std::unique_ptr<Expr> in parent nodes.
 */
struct Expr {
    virtual ~Expr() = default;
    SourcePos pos{};
};

} // namespace gwbasic
