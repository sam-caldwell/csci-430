// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/SourcePos.h"

namespace gwbasic {

/**
 * Type: Stmt (abstract)
 * Purpose:
 *  - Common base for all statements (PRINT, LET/assignment, control flow,
 *    loops, subroutine calls, etc.).
 * Inputs:
 *  - pos: Source position captured by the parser for diagnostics/logging
 * Outputs:
 *  - Virtual base enabling polymorphic statement handling
 * Theory of operation:
 *  - Concrete statements subclass Stmt and are stored via
 *    std::unique_ptr<Stmt> in containing structures.
 */
struct Stmt {
    virtual ~Stmt() = default;
    SourcePos pos{};
};

} // namespace gwbasic
