// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_OPTIONPRINTZONESSTMT_H
#define BASIC_COMPILER_AST_OPTIONPRINTZONESSTMT_H

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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

#endif // BASIC_COMPILER_AST_OPTIONPRINTZONESSTMT_H
