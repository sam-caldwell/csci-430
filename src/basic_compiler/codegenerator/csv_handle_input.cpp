// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/InputStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleInput
 * Summary: Collect variables and prompt literal for INPUT.
 * Parameters:
 *  - inputStmt: Parsed InputStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleInput(const InputStmt* inputStmt) {

    for (const auto& varName : inputStmt->variables) {
        variables_.insert(varName);
    }

    if (inputStmt->promptLiteral && !strLiteralId_.contains(*inputStmt->promptLiteral)) {
        strLiteralId_[*inputStmt->promptLiteral] = strCounter_++;
    }

    logSem() << "Input vars=" << inputStmt->variables.size() << " @ "
             << inputStmt->pos.line << ':' << inputStmt->pos.col << Symbols::LF;
}

} // namespace gwbasic
