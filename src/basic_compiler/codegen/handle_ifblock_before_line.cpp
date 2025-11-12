// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_ifblock_before_line.cpp
 * Purpose: Implement CodeGenerator::handleIfBlockBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

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
bool CodeGenerator::handleIfBlockBeforeLine(const Stmt *s,
                                            std::set<std::string, std::less<>> &vars,
                                            std::set<std::string, std::less<>> &arrays) {
    const auto ib = dyn_cast<const IfBlockStmt>(s);
    if (!ib) return false;
    collectVarsForBeforeLineFromExpr(ib->cond.get(), vars, arrays);
    for (const auto &st: ib->thenBody) collectVarsForBeforeLineFromStmt(st.get(), vars, arrays);
    for (const auto &st: ib->elseBody) collectVarsForBeforeLineFromStmt(st.get(), vars, arrays);
    return true;
}
