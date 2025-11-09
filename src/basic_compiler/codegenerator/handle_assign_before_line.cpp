// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_assign_before_line.cpp
 * Purpose: Implement CodeGenerator::handleAssignBeforeLine for before-line var collection.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

using namespace gwbasic;

/*
 * Function: handleAssignBeforeLine
 * Summary: Collect names for an assignment before-line snapshot.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleAssignBeforeLine(const Stmt *s,
                                           std::set<std::string, std::less<>> &vars,
                                           std::set<std::string, std::less<>> &arrays) {
    const auto a = dyn_cast<const AssignStmt>(s);
    if (!a) return false;
    vars.insert(a->name);
    collectVarsForBeforeLineFromExpr(a->value.get(), vars, arrays);
    return true;
}
