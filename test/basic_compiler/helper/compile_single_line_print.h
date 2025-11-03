// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include "basic_compiler/Compiler.h"

/* Helper: compileSingleLinePrint
 * Purpose: Compile a minimal program that prints an expression and returns IR.
 * Components Under Test: Compiler; CodeGenerator
 * Expected Behavior: Returns IR text for a program printing the given expression.
 */
inline std::string compileSingleLinePrint(const std::string& expr) {
    const auto src = std::string("10 PRINT ") + expr + "\n20 END\n";
    return gwbasic::Compiler::compileString(src);
}

