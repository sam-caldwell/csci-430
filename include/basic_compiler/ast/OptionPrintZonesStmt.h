// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: OptionPrintZonesStmt
 * Purpose:
 *  - OPTION PRINTZONES ON|OFF directive to enable/disable PRINT comma-zone padding.
 */
struct OptionPrintZonesStmt : ASTLeaf<NodeKind::OptionPrintZonesStmt, Stmt> {
    bool enabled{false};
    OptionPrintZonesStmt() = default;
    explicit OptionPrintZonesStmt(bool e) : ASTLeaf(), enabled(e) {}
};

} // namespace gwbasic

