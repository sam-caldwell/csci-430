// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_read_before_line.cpp
 * Purpose: Implement CodeGenerator::handleReadBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleReadBeforeLine
 * Summary: Collect names referenced by READ before a line.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleReadBeforeLine(const Stmt* stmt,
                                         std::set<std::string, std::less<>>& vars,
                                         std::set<std::string, std::less<>>& arrays) {
    const auto* const readStmt = dyn_cast<const ReadStmt>(stmt);
    if (readStmt == nullptr) {
        return false;
    }

    for (const auto& [name, indices] : readStmt->targets) {
        if (indices.empty()) {
            vars.insert(name);
            continue;
        }
        arrays.insert(name);
        for (const auto& indexExpr : indices) {
            collectVarsForBeforeLineFromExpr(indexExpr.get(), vars, arrays);
        }
    }
    return true;
}
