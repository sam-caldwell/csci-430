// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_NEXTSTMT_H
#define BASIC_COMPILER_AST_NEXTSTMT_H

#include <string>
#include <vector>

#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Type: NextStmt
 * Purpose:
 *  - Marker statement delimiting the end of a FOR block in multi-line form.
 * Inputs:
 *  - vars: Optional list of induction variable names (NEXT var[,var...]).
 *          An empty list represents a bare NEXT which closes the innermost
 *          FOR only. When multiple variables are present, each must match
 *          the current innermost FOR in order, and each closes one level
 *          of nesting (equivalent to "NEXT v1 : NEXT v2 : ...").
 * Outputs:
 *  - Concrete Stmt recognized by the parser; eliminated during parseProgram
 *    restructuring when FOR bodies are folded.
 */
struct NextStmt : ASTLeaf<NodeKind::NextStmt, Stmt> {
    std::vector<std::string> vars; // may be empty to indicate bare NEXT
    NextStmt() = default;
    explicit NextStmt(std::vector<std::string> v) : ASTLeaf(), vars(std::move(v)) {}
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_NEXTSTMT_H
