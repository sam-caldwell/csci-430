// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_DEFTYPESTMT_H
#define BASIC_COMPILER_AST_DEFTYPESTMT_H

#include <cstdint>
#include <utility>
#include <vector>

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

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
    enum class Kind : std::uint8_t { Int, Sng, Dbl, Str };
    Kind kind;
    std::vector<std::pair<char,char>> ranges; // inclusive, uppercase
    DefTypeStmt(const Kind k, std::vector<std::pair<char,char>> r)
        : ASTLeaf(), kind(k), ranges(std::move(r)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_DEFTYPESTMT_H
