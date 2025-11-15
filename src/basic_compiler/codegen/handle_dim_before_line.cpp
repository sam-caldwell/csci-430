// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_dim_before_line.cpp
 * Purpose: Implement CodeGenerator::handleDimBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DimStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

/*
 * Function: handleDimBeforeLine
 * Summary: Collect array name from DIM before-line snapshot.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - arrays: Output set of array names referenced.
 * Returns:
 *  - bool: True if the statement was handled.
 */
bool CodeGenerator::handleDimBeforeLine(const Stmt* stmt,
                                        std::set<std::string, std::less<>>& arrays) {
    const auto* const dim = dyn_cast<const DimStmt>(stmt);
    if (dim == nullptr) {
        return false;
    }
    arrays.insert(dim->name);
    return true;
}
