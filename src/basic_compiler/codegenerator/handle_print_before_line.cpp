// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_print_before_line.cpp
 * Purpose: Implement CodeGenerator::handlePrintBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

using namespace gwbasic;

/*
 * Function: handlePrintBeforeLine
 * Summary: Collect names referenced by PRINT before a line.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handlePrintBeforeLine(const Stmt *s,
                                          std::set<std::string, std::less<>> &vars,
                                          std::set<std::string, std::less<>> &arrays) {

    const auto pr = dyn_cast<const PrintStmt>(s);

    if (!pr)
        return false;

    if (pr->value)
        collectVarsForBeforeLineFromExpr(pr->value.get(), vars, arrays);

    for (const auto &v: pr->more)
        collectVarsForBeforeLineFromExpr(v.get(), vars, arrays);

    return true;
}
