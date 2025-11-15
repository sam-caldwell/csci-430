// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_ifblock_before_line.cpp
 * Purpose: Implement CodeGenerator::handleIfBlockBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleIfBlockBeforeLine
 * Summary: Collect names for a multi-statement IF block before a line.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleIfBlockBeforeLine(const Stmt* stmt,
                                            std::set<std::string, std::less<>>& vars,
                                            std::set<std::string, std::less<>>& arrays) {
    const auto* const ifBlock = dyn_cast<const IfBlockStmt>(stmt);
    if (ifBlock == nullptr) {
        return false;
    }
    collectVarsForBeforeLineFromExpr(ifBlock->cond.get(), vars, arrays);
    for (const auto& subStmt : ifBlock->thenBody) {
        collectVarsForBeforeLineFromStmt(subStmt.get(), vars, arrays);
    }
    for (const auto& subStmt : ifBlock->elseBody) {
        collectVarsForBeforeLineFromStmt(subStmt.get(), vars, arrays);
    }
    return true;
}
