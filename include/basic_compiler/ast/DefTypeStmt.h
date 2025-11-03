// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <vector>
#include <utility>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: DefTypeStmt
 * Purpose:
 *  - DEFINT/DEFSNG/DEFDBL/DEFSTR A-C,X,Z style default type declarations.
 * Data:
 *  - kind: one of Int, Sng, Dbl, Str
 *  - ranges: list of inclusive letter ranges (uppercase A..Z)
 */
struct DefTypeStmt : ASTLeaf<NodeKind::DefTypeStmt, Stmt> {
    enum class Kind { Int, Sng, Dbl, Str };
    Kind kind;
    std::vector<std::pair<char,char>> ranges; // inclusive, uppercase
    DefTypeStmt(const Kind k, std::vector<std::pair<char,char>> r)
        : ASTLeaf(), kind(k), ranges(std::move(r)) {}
};

} // namespace gwbasic

