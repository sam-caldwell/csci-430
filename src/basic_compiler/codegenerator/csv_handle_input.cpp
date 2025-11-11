// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/InputStmt.h"

namespace gwbasic {

/*
 * Function: csvHandleInput
 * Summary: Collect variables and prompt literal for INPUT.
 * Parameters:
 *  - input_stmt: Parsed InputStmt node.
 * Returns:
 *  - void
 */
void CodeGenerator::csvHandleInput(const InputStmt* input_stmt) {

    for (const auto& varName : input_stmt->variables) {
        variables_.insert(varName);
    }

    if (input_stmt->promptLiteral && !strLiteralId_.contains(*input_stmt->promptLiteral)) {
        strLiteralId_[*input_stmt->promptLiteral] = strCounter_++;
    }

    logSem() << "Input vars=" << input_stmt->variables.size() << " @ "
             << input_stmt->pos.line << ':' << input_stmt->pos.col << Symbols::LF;
}

} // namespace gwbasic
