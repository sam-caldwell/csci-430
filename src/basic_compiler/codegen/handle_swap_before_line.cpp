// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_swap_before_line.cpp
 * Purpose: Implement CodeGenerator::handleSwapBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/SwapStmt.h"
#include "basic_compiler/ast/ReadTarget.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleSwapBeforeLine
 * Summary: Collect names referenced by SWAP before a line.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleSwapBeforeLine(const Stmt* stmt,
                                         std::set<std::string, std::less<>>& vars,
                                         std::set<std::string, std::less<>>& arrays) {
    const auto* const swapStmt = dyn_cast<const SwapStmt>(stmt);
    if (swapStmt == nullptr) {
        return false;
    }

    auto handle = [&](const ReadTarget& target) {
        if (!target.indices.empty()) {
            arrays.insert(target.name);
            for (const auto& indexExpr : target.indices) {
                collectVarsForBeforeLineFromExpr(indexExpr.get(), vars, arrays);
            }
        } else {
            vars.insert(target.name);
        }
    };

    handle(swapStmt->left);
    handle(swapStmt->right);

    return true;
}
