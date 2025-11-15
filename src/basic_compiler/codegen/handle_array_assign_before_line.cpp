// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_array_assign_before_line.cpp
 * Purpose: Implement CodeGenerator::handleArrayAssignBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleArrayAssignBeforeLine
 * Summary: Collect names for an array assignment before-line snapshot.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleArrayAssignBeforeLine(const Stmt* stmt,
                                                std::set<std::string, std::less<>>& vars,
                                                std::set<std::string, std::less<>>& arrays) {
    const auto* const assign = dyn_cast<const ArrayAssignStmt>(stmt);
    if (assign == nullptr) {
        return false;
    }
    arrays.insert(assign->name);
    for (const auto& indexExpr : assign->indices) {
        collectVarsForBeforeLineFromExpr(indexExpr.get(), vars, arrays);
    }
    collectVarsForBeforeLineFromExpr(assign->value.get(), vars, arrays);
    return true;
}
