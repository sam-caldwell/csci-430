// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/* Collect variables for INPUT statement. */
void CodeGenerator::csvHandleInput(const InputStmt* in) {

    for (const auto& v : in->variables)
        variables_.insert(v);

    if (in->promptLiteral && !strLiteralId_.contains(*in->promptLiteral))
        strLiteralId_[*in->promptLiteral] = strCounter_++;

    logSem() << "Input vars=" << in->variables.size() << " @ " << in->pos.line << ':' << in->pos.col << Symbols::LF;
}

} // namespace gwbasic

