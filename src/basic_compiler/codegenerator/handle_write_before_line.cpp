// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_write_before_line.cpp
 * Purpose: Implement CodeGenerator::handleWriteBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/WriteStmt.h"

using namespace gwbasic;

bool CodeGenerator::handleWriteBeforeLine(const Stmt *s,
                                          std::set<std::string, std::less<>> &vars,
                                          std::set<std::string, std::less<>> &arrays) {
    const auto wr = dyn_cast<const WriteStmt>(s);
    if (!wr) return false;
    for (const auto &e: wr->items) collectVarsForBeforeLineFromExpr(e.get(), vars, arrays);
    return true;
}

