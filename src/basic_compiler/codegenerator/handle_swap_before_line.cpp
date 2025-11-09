// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_swap_before_line.cpp
 * Purpose: Implement CodeGenerator::handleSwapBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/SwapStmt.h"

using namespace gwbasic;

bool CodeGenerator::handleSwapBeforeLine(const Stmt *s,
                                         std::set<std::string, std::less<>> &vars,
                                         std::set<std::string, std::less<>> &arrays) {
    const auto sw = dyn_cast<const SwapStmt>(s);
    if (!sw) return false;
    auto handle = [&](const ReadTarget &t) {
        if (!t.indices.empty()) {
            arrays.insert(t.name);
            for (const auto &ix: t.indices) collectVarsForBeforeLineFromExpr(ix.get(), vars, arrays);
        } else {
            vars.insert(t.name);
        }
    };
    handle(sw->left);
    handle(sw->right);
    return true;
}
