// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_erase_before_line.cpp
 * Purpose: Implement CodeGenerator::handleEraseBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/EraseStmt.h"
#include "basic_compiler/ast/RTTI.h"

using namespace gwbasic;

bool CodeGenerator::handleEraseBeforeLine(const Stmt *s,
                                          std::set<std::string, std::less<>> &arrays) {
    const auto er = dyn_cast<const EraseStmt>(s);
    if (!er) return false;
    for (const auto &n: er->names) arrays.erase(n);
    return true;
}
