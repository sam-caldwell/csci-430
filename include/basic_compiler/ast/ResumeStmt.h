// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: ResumeStmt
 * Purpose:
 *  - Nonlocal continuation from an error handler context.
 * Forms:
 *  - RESUME            (re-exec failing statement)
 *  - RESUME 0          (synonym for RESUME)
 *  - RESUME NEXT       (continue at statement after failing statement)
 *  - RESUME <line>     (branch to specified line)
 */
struct ResumeStmt : ASTLeaf<NodeKind::ResumeStmt, Stmt> {
    enum class Kind { Reexecute, Next, Line };
    Kind kind{Kind::Reexecute};
    int line{0}; // used only when kind==Line
    ResumeStmt() : ASTLeaf() {}
    static ResumeStmt Reexec() { return ResumeStmt(); }
    static ResumeStmt Next() { ResumeStmt r; r.kind = Kind::Next; return r; }
    static ResumeStmt ToLine(int ln) { ResumeStmt r; r.kind = Kind::Line; r.line = ln; return r; }
};

} // namespace gwbasic

