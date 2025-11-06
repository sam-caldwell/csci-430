// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"

namespace gwbasic {

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
void CodeGenerator::collectStmtVars(const Stmt* s) {
    if (const auto p = dyn_cast<const PrintStmt>(s)) {
        if (p->value) {
            const Expr* v = p->value.get();
            collectExprVars(v);
            if (const auto se = dyn_cast<StringExpr>(v)) {
                if (!strLiteralId_.count(se->value)) strLiteralId_[se->value] = strCounter_++;
                logSem() << "StringLiteral @ " << se->pos.line << ':' << se->pos.col << Symbols::LF;
            }
        }
        for (const auto& vx : p->more) {
            const Expr* v = vx.get();
            collectExprVars(v);
            if (const auto se = dyn_cast<StringExpr>(v)) {
                if (!strLiteralId_.count(se->value)) strLiteralId_[se->value] = strCounter_++;
                logSem() << "StringLiteral @ " << se->pos.line << ':' << se->pos.col << Symbols::LF;
            }
        }
    } else if (const auto a = dyn_cast<const AssignStmt>(s)) {
        variables_.insert(a->name);
        collectExprVars(a->value.get());
        logSem() << "Assign " << a->name << " @ " << a->pos.line << ':' << a->pos.col << Symbols::LF;
    } else if (const auto m = dyn_cast<const MidAssignStmt>(s)) {
        variables_.insert(m->name);
        // collect index expressions for array element targets, if any
        for (const auto& ix : m->indices) collectExprVars(ix.get());
        collectExprVars(m->start.get());
        if (m->len) collectExprVars(m->len.get());
        collectExprVars(m->value.get());
        logSem() << "MidAssign " << m->name << " @ " << m->pos.line << ':' << m->pos.col << Symbols::LF;
    } else if (const auto i = dyn_cast<const IfStmt>(s)) {
        collectExprVars(i->cond.get());
        logSem() << "If @ " << i->pos.line << ':' << i->pos.col << Symbols::LF;
    } else if (const auto f = dyn_cast<const ForStmt>(s)) {
        variables_.insert(f->var);
        collectExprVars(f->start.get());
        collectExprVars(f->end.get());
        if (f->step) collectExprVars(f->step.get());
        for (const auto& bs : f->body) collectStmtVars(bs.get());
        logSem() << "For var=" << f->var << " @ " << f->pos.line << ':' << f->pos.col << Symbols::LF;
    } else if (const auto in = dyn_cast<const InputStmt>(s)) {
        variables_.insert(in->name);
        logSem() << "Input " << in->name << " @ " << in->pos.line << ':' << in->pos.col << Symbols::LF;
    } else if (const auto rz = dyn_cast<const RandomizeStmt>(s)) {
        if (rz->seed) collectExprVars(rz->seed.get());
        logSem() << "Randomize @ " << rz->pos.line << ':' << rz->pos.col << Symbols::LF;
    } else if (const auto cs = dyn_cast<const CommonStmt>(s)) {
        for (const auto& n : cs->names) {
            variables_.insert(n);
            commonVariables_.insert(n);
            logSem() << "Common " << n << " @ " << cs->pos.line << ':' << cs->pos.col << Symbols::LF;
        }
    } else if (dyn_cast<const MergeStmt>(s)) {
        // MERGE is a compile-time directive; codegen no-op
    } else if (const auto ds = dyn_cast<const DataStmt>(s)) {
        // Normalize DATA items into string literals and record ids
        for (const auto& it : ds->items) {
            const std::string& txt = it.text;
            if (!strLiteralId_.count(txt)) strLiteralId_[txt] = strCounter_++;
            dataLiteralIds_.push_back(strLiteralId_[txt]);
        }
        logSem() << "Data items=" << ds->items.size() << Symbols::LF;
    } else if (const auto rd = dyn_cast<const ReadStmt>(s)) {
        for (const auto& t : rd->targets) {
            if (!t.indices.empty()) {
                for (const auto& ix : t.indices) collectExprVars(ix.get());
            } else {
                variables_.insert(t.name);
            }
        }
    } else if (const auto og = dyn_cast<const OnGotoStmt>(s)) {
        collectExprVars(og->index.get());
        logSem() << "OnGoto targets=" << og->targets.size() << Symbols::LF;
    } else if (const auto ogs = dyn_cast<const OnGosubStmt>(s)) {
        collectExprVars(ogs->index.get());
        logSem() << "OnGosub targets=" << ogs->targets.size() << Symbols::LF;
    }
}

} // namespace gwbasic
