// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_ongosub_before_line.cpp
 * Purpose: Implement CodeGenerator::handleOnGosubBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/OnGosubStmt.h"

using namespace gwbasic;

bool CodeGenerator::handleOnGosubBeforeLine(const Stmt *s,
                                            std::set<std::string, std::less<>> &vars,
                                            std::set<std::string, std::less<>> &arrays) {
    const auto ogs = dyn_cast<const OnGosubStmt>(s);
    if (!ogs) return false;
    collectVarsForBeforeLineFromExpr(ogs->index.get(), vars, arrays);
    return true;
}

