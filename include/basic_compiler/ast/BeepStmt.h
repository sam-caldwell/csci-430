// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/** BeepStmt: BEEP */
struct BeepStmt final : ASTLeaf<NodeKind::BeepStmt, Stmt> {
    BeepStmt() = default;
};

} // namespace gwbasic

