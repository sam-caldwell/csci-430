// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/* Collect variables for FOR header and nested body. */
void CodeGenerator::csvHandleFor(const ForStmt* f) {
    variables_.insert(f->var);
    collectExprVars(f->start.get());
    collectExprVars(f->end.get());
    if (f->step)
        collectExprVars(f->step.get());
    for (const auto& bs : f->body)
        collectStmtVars(bs.get());
    logSem() << "For var=" << f->var << " @ " << f->pos.line << ':' << f->pos.col << Symbols::LF;
}

} // namespace gwbasic

