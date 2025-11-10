// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_UNSUPPORTEDSTMT_H
#define BASIC_COMPILER_AST_UNSUPPORTEDSTMT_H

#include <string>

#include "basic_compiler/ast/Node.h"
#include "basic_compiler/ast/NodeKind.h"
#include "basic_compiler/ast/NodeTemplate.h"
#include "basic_compiler/ast/SourcePos.h"
#include "basic_compiler/ast/Stmt.h"

namespace gwbasic {

/**
 * Struct: UnsupportedStmt
 * Purpose:
 *  - Placeholder AST node for keywords that are recognized lexically but
 *    not yet implemented in parser/semantics/codegen. Enables grammar
 *    coverage and future implementation without rejecting programs.
 * Fields:
 *  - keyword: Uppercased keyword text (e.g., "FILES", "CLS").
 */
struct UnsupportedStmt : ASTLeaf<NodeKind::UnsupportedStmt, Stmt> {
    std::string keyword;
    UnsupportedStmt() = default;
    UnsupportedStmt(const SourcePos& p, std::string kw) : ASTLeaf<NodeKind::UnsupportedStmt, Stmt>(p), keyword(std::move(kw)) {}
    static bool classof(const Node* N) { return N && N->kind == NodeKind::UnsupportedStmt; }
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_UNSUPPORTEDSTMT_H
