// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_array_assign_before_line.cpp
 * Purpose: Implement CodeGenerator::handleArrayAssignBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"

using namespace gwbasic;

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

