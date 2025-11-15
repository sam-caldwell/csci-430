// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_ongoto_before_line.cpp
 * Purpose: Implement CodeGenerator::handleOnGotoBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleOnGotoBeforeLine
 * Summary: Collect names referenced by ON ... GOTO before a line.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleOnGotoBeforeLine(const Stmt* stmt,
                                           std::set<std::string, std::less<>>& vars,
                                           std::set<std::string, std::less<>>& arrays) {
    const auto* const onGoto = dyn_cast<const OnGotoStmt>(stmt);
    if (onGoto == nullptr) {
        return false;
    }
    collectVarsForBeforeLineFromExpr(onGoto->index.get(), vars, arrays);
    return true;
}
