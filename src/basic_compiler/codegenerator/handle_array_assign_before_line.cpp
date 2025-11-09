// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_array_assign_before_line.cpp
 * Purpose: Implement CodeGenerator::handleArrayAssignBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

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
bool CodeGenerator::handleArrayAssignBeforeLine(const Stmt *s,
                                                std::set<std::string, std::less<>> &vars,
                                                std::set<std::string, std::less<>> &arrays) {
    const auto aa = dyn_cast<const ArrayAssignStmt>(s);
    if (!aa) return false;
    arrays.insert(aa->name);
    for (const auto &ix: aa->indices)
        collectVarsForBeforeLineFromExpr(ix.get(), vars, arrays);
    collectVarsForBeforeLineFromExpr(aa->value.get(), vars, arrays);
    return true;
}
