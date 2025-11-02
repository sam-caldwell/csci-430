// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

struct WendStmt : ASTLeaf<NodeKind::WendStmt, Stmt> { WendStmt() = default; };

} // namespace gwbasic

