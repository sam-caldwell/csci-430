// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

void CodeGenerator::collectStmtVars(const Stmt* s) {
    /*
     * Function: CodeGenerator::collectStmtVars
     * Inputs:
     *  - s: statement node to analyze
     * Outputs:
     *  - void (updates internal sets/maps of variables and string literals)
     * Theory of operation:
     *  - Inspects the statement kind to discover referenced variables and
     *    string constants, recursing into contained expressions/blocks.
     */
    if (const auto p = dynamic_cast<const PrintStmt*>(s)) {
        collectExprVars(p->value.get());
        if (const auto se = dynamic_cast<StringExpr*>(p->value.get())) {
            if (!strLiteralId_.count(se->value)) strLiteralId_[se->value] = strCounter_++;
            { std::ostringstream m; m << "StringLiteral @ " << se->pos.line << ':' << se->pos.col; logSem(m.str()); }
        }
    } else if (const auto a = dynamic_cast<const AssignStmt*>(s)) {
        variables_.insert(a->name);
        collectExprVars(a->value.get());
        { std::ostringstream m; m << "Assign " << a->name << " @ " << a->pos.line << ':' << a->pos.col; logSem(m.str()); }
    } else if (const auto i = dynamic_cast<const IfStmt*>(s)) {
        collectExprVars(i->cond.get());
        { std::ostringstream m; m << "If @ " << i->pos.line << ':' << i->pos.col; logSem(m.str()); }
    } else if (const auto f = dynamic_cast<const ForStmt*>(s)) {
        variables_.insert(f->var);
        collectExprVars(f->start.get());
        collectExprVars(f->end.get());
        if (f->step) collectExprVars(f->step.get());
        for (const auto& bs : f->body) collectStmtVars(bs.get());
        { std::ostringstream m; m << "For var=" << f->var << " @ " << f->pos.line << ':' << f->pos.col; logSem(m.str()); }
    } else if (const auto in = dynamic_cast<const InputStmt*>(s)) {
        variables_.insert(in->name);
        { std::ostringstream m; m << "Input " << in->name << " @ " << in->pos.line << ':' << in->pos.col; logSem(m.str()); }
    }
}

} // namespace gwbasic
