// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_RESUMESTMT_H
#define BASIC_COMPILER_AST_RESUMESTMT_H

#include <cstdint>

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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
    enum class Kind : std::uint8_t { Reexecute, Next, Line };
    Kind kind{Kind::Reexecute};
    int line{0}; // used only when kind==Line
    ResumeStmt() : ASTLeaf() {}
    explicit ResumeStmt(Kind k) : ASTLeaf(), kind(k) {}
    ResumeStmt(Kind k, int ln) : ASTLeaf(), kind(k), line(ln) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_RESUMESTMT_H
