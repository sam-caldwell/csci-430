// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_read_before_line.cpp
 * Purpose: Implement CodeGenerator::handleReadBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

using namespace gwbasic;

bool CodeGenerator::handleReadBeforeLine(const Stmt *s,
                                         std::set<std::string, std::less<>> &vars,
                                         std::set<std::string, std::less<>> &arrays) {
    const auto rd = dyn_cast<const ReadStmt>(s);
    if (!rd) return false;
    for (const auto &[name, indices]: rd->targets) {
        if (!indices.empty()) {
            arrays.insert(name);
            for (const auto &ix: indices) collectVarsForBeforeLineFromExpr(ix.get(), vars, arrays);
        } else {
            vars.insert(name);
        }
    }
    return true;
}
