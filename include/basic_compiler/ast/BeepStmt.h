#ifndef BASIC_COMPILER_AST_BEEPSTMT_H
#define BASIC_COMPILER_AST_BEEPSTMT_H

// (c) 2025 Sam Caldwell. All Rights Reserved.

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/** BeepStmt: BEEP */
struct BeepStmt final : ASTLeaf<NodeKind::BeepStmt, Stmt> {
    BeepStmt() = default;
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_BEEPSTMT_H
