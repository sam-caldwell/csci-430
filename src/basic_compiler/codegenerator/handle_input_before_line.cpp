// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: handle_input_before_line.cpp
 * Purpose: Implement CodeGenerator::handleInputBeforeLine.
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"

using namespace gwbasic;

/*
 * Function: handleInputBeforeLine
 * Summary: Collect input variable names before a line.
 * Parameters:
 *  - s: Statement pointer to inspect.
 *  - vars: Output set of scalar variable names.
 *  - arrays: Optional output set of array names (unused).
 * Returns:
 *  - bool: True if the statement was handled.
 */
auto CodeGenerator::handleInputBeforeLine(const Stmt *s,
                                          std::set<std::string, std::less<>> &vars,
                                          std::set<std::string, std::less<>> *arrays) -> bool {
    const auto in = dyn_cast<const InputStmt>(s);
    if (!in) return false;
    (void)arrays; // unused here
    for (const auto &v: in->variables) vars.insert(v);
    return true;
}
