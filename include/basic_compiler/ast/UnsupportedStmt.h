// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_UNSUPPORTEDSTMT_H
#define BASIC_COMPILER_AST_UNSUPPORTEDSTMT_H

#include <string>
#include <utility>

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
    UnsupportedStmt(const SourcePos& pos, std::string keyword_str)
        : ASTLeaf<NodeKind::UnsupportedStmt, Stmt>(pos), keyword(std::move(keyword_str)) {}
    static bool classof(const Node* node_ptr) {
        return node_ptr != nullptr && node_ptr->kind == NodeKind::UnsupportedStmt;
    }
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_UNSUPPORTEDSTMT_H
