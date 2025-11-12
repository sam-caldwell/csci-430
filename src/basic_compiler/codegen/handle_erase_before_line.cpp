// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_erase_before_line.cpp
 * Purpose: Implement CodeGenerator::handleEraseBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/EraseStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleEraseBeforeLine
 * Summary: Remove array names erased by ERASE before a line.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - arrays: In/out set of array names.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleEraseBeforeLine(const Stmt* stmt,
                                          std::set<std::string, std::less<>>& arrays) {
    const auto* eraseStmt = dyn_cast<const EraseStmt>(stmt);
    if (eraseStmt == nullptr) {
        return false;
    }
    for (const auto& name : eraseStmt->names) {
        arrays.erase(name);
    }
    return true;
}
