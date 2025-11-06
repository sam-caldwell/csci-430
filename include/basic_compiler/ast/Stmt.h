// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Node.h"

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
struct Stmt : Node {
    ~Stmt() override = default;
protected:
    explicit Stmt(NodeKind k) : Node(k) {}
    Stmt() : Node(NodeKind::AbstractStmt) {}
public:
    static bool classof(const Node* N) {
        if (!N) return false;
        switch (N->kind) {
            case NodeKind::AbstractStmt:
            case NodeKind::AssignStmt:
            case NodeKind::PrintStmt:
            case NodeKind::GotoStmt:
            case NodeKind::GosubStmt:
            case NodeKind::ReturnStmt:
            case NodeKind::IfStmt:
            case NodeKind::IfBlockStmt:
            case NodeKind::ElseStmt:
            case NodeKind::EndIfStmt:
            case NodeKind::InputStmt:
            case NodeKind::ForStmt:
            case NodeKind::NextStmt:
            case NodeKind::EndStmt:
            case NodeKind::StopStmt:
            case NodeKind::SystemStmt:
            case NodeKind::RandomizeStmt:
            case NodeKind::WhileStmt:
            case NodeKind::WendStmt:
            case NodeKind::RunStmt:
            case NodeKind::CommonStmt:
            case NodeKind::ChainStmt:
            case NodeKind::MergeStmt:
            case NodeKind::DimStmt:
            case NodeKind::ArrayAssignStmt:
            case NodeKind::MidAssignStmt:
                return true;
            default:
                return false;
        }
    }
};

} // namespace gwbasic
