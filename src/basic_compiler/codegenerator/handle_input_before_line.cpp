// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_input_before_line.cpp
 * Purpose: Implement CodeGenerator::handleInputBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/InputStmt.h"

using namespace gwbasic;

bool CodeGenerator::handleInputBeforeLine(const Stmt *s,
                                          std::set<std::string, std::less<>> &vars,
                                          std::set<std::string, std::less<>> &arrays) {
    const auto in = dyn_cast<const InputStmt>(s);
    if (!in) return false;
    for (const auto &v: in->variables) vars.insert(v);
    return true;
}

