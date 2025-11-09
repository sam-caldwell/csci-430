// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_dim_before_line.cpp
 * Purpose: Implement CodeGenerator::handleDimBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/DimStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

using namespace gwbasic;

bool CodeGenerator::handleDimBeforeLine(const Stmt *s,
                                        std::set<std::string, std::less<>> &arrays) {
    const auto ds = dyn_cast<const DimStmt>(s);
    if (!ds) return false;
    arrays.insert(ds->name);
    return true;
}
