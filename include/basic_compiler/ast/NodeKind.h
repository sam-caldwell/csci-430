// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

namespace gwbasic {

/**
 * Enum: NodeKind
 *  - Lightweight identifier for AST node kinds enabling LLVM-style RTTI.
 */
enum class NodeKind {
    // Abstract/base markers
    AbstractExpr,
    AbstractStmt,
    // Expressions
    NumberExpr,
    StringExpr,
    VarExpr,
    UnaryExpr,
    BinaryExpr,
    CallExpr,
    // Statements
    AssignStmt,
    PrintStmt,
    GotoStmt,
    GosubStmt,
    ReturnStmt,
    IfStmt,
    InputStmt,
    ForStmt,
    EndStmt,
    RandomizeStmt,
};

} // namespace gwbasic
