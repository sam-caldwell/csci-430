// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <vector>
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NodeTemplate.h"

namespace gwbasic {

/**
 * Type: FileInputStmt
 * Purpose:
 *  - INPUT #n, var[, var...] — read numeric values from a file channel.
 * Inputs:
 *  - channel: File channel number (1..16)
 *  - variables: List of destination variable names
 * Outputs:
 *  - Concrete Stmt node; semantics may validate identifiers
 */
struct FileInputStmt : ASTLeaf<NodeKind::FileInputStmt, Stmt> {
    int channel{1};
    std::vector<std::string> variables; // names only; array targets not yet supported here
    FileInputStmt() = default;

    explicit FileInputStmt(const int ch, std::vector<std::string> vars)
        : ASTLeaf(), channel(ch), variables(std::move(vars)) {}
};

} // namespace gwbasic
