// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_write_before_line.cpp
 * Purpose: Implement CodeGenerator::handleWriteBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleWriteBeforeLine
 * Summary: Collect names referenced by WRITE before a line.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleWriteBeforeLine(const Stmt *stmt,
                                          std::set<std::string, std::less<>> &vars,
                                          std::set<std::string, std::less<>> &arrays) {
    const auto *const writeStmt = dyn_cast<const WriteStmt>(stmt);
    if (writeStmt == nullptr) {
        return false;
    }
    for (const auto &exprPtr : writeStmt->items) {
        collectVarsForBeforeLineFromExpr(exprPtr.get(), vars, arrays);
    }
    return true;
}
