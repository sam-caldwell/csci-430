// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_ongosub_before_line.cpp
 * Purpose: Implement CodeGenerator::handleOnGosubBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <functional>
#include <set>
#include <string>

using namespace gwbasic;

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
