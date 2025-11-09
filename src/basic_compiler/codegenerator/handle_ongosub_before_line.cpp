// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_ongosub_before_line.cpp
 * Purpose: Implement CodeGenerator::handleOnGosubBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleOnGosubBeforeLine
 * Summary: Collect names referenced by ON ... GOSUB before a line.
 * Parameters:
 *  - stmt: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleOnGosubBeforeLine(const Stmt* stmt,
                                            std::set<std::string, std::less<>>& vars,
                                            std::set<std::string, std::less<>>& arrays) {
    const auto* const onGosub = dyn_cast<const OnGosubStmt>(stmt);
    if (onGosub == nullptr) {
        return false;
    }
    collectVarsForBeforeLineFromExpr(onGosub->index.get(), vars, arrays);
    return true;
}
