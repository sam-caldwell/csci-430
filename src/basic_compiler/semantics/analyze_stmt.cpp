// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <sstream>

namespace gwbasic {

void SemanticAnalyzer::analyzeStmt(const Stmt* s) {
    if (auto p = dyn_cast<const PrintStmt>(s)) { analyzeExpr(p->value.get()); return; }
    if (auto a = dyn_cast<const AssignStmt>(s)) {
        reference(a->name, a->pos);
        if (typeOf(a->value.get()) == ValueType::String) {
            std::ostringstream m; m << "TypeError: cannot assign string to numeric var '" << a->name << "' @ " << a->pos.line << ':' << a->pos.col; log(m.str());
            throw SemanticError(m.str());
        }
        analyzeExpr(a->value.get());
        return;
    }
    if (auto i = dyn_cast<const IfStmt>(s)) {
        if (typeOf(i->cond.get()) == ValueType::String) {
            std::ostringstream m; m << "TypeError: IF condition cannot be string @ " << i->pos.line << ':' << i->pos.col; log(m.str());
            throw SemanticError(m.str());
        }
        analyzeExpr(i->cond.get());
        if (!lines_.contains(i->targetLine)) {
            if (strictControlFlow_) {
                std::ostringstream err; err << "ControlFlowError: missing IF target line " << i->targetLine << " @ " << i->pos.line << ':' << i->pos.col; log(err.str());
                throw SemanticError(err.str());
            } else {
                std::ostringstream w; w << "Warning: IF missing target line " << i->targetLine << " @ " << i->pos.line << ':' << i->pos.col; log(w.str());
            }
        }
        std::ostringstream m; m << "IfThen target=" << i->targetLine << " @ " << i->pos.line << ':' << i->pos.col; log(m.str());
        return;
    }
    if (auto f = dyn_cast<const ForStmt>(s)) {
        reference(f->var, f->pos);
        std::ostringstream m; m << "For var=" << f->var << " @ " << f->pos.line << ':' << f->pos.col; log(m.str());
        if (typeOf(f->start.get()) == ValueType::String) { std::ostringstream err; err << "TypeError: FOR start must be numeric @ " << f->pos.line << ':' << f->pos.col; log(err.str()); throw SemanticError(err.str()); }
        if (typeOf(f->end.get()) == ValueType::String) { std::ostringstream err; err << "TypeError: FOR end must be numeric @ " << f->pos.line << ':' << f->pos.col; log(err.str()); throw SemanticError(err.str()); }
        if (f->step && typeOf(f->step.get()) == ValueType::String) { std::ostringstream err; err << "TypeError: FOR step must be numeric @ " << f->pos.line << ':' << f->pos.col; log(err.str()); throw SemanticError(err.str()); }
        analyzeExpr(f->start.get());
        analyzeExpr(f->end.get());
        analyzeExpr(f->step.get());
        enterScope();
        for (const auto& bs : f->body) analyzeStmt(bs.get());
        exitScope();
        return;
    }
    if (auto in = dyn_cast<const InputStmt>(s)) { reference(in->name, in->pos); return; }
    if (auto g = dyn_cast<const GotoStmt>(s)) {
        std::ostringstream m; m << "Goto target=" << g->targetLine << " @ " << g->pos.line << ':' << g->pos.col; log(m.str());
        if (!lines_.contains(g->targetLine)) {
            if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing GOTO target line " << g->targetLine << " @ " << g->pos.line << ':' << g->pos.col; log(err.str()); throw SemanticError(err.str()); }
            else { std::ostringstream w; w << "Warning: GOTO missing target line " << g->targetLine << " @ " << g->pos.line << ':' << g->pos.col; log(w.str()); }
        }
        return;
    }
    if (auto gs = dyn_cast<const GosubStmt>(s)) {
        std::ostringstream m; m << "Gosub target=" << gs->targetLine << " @ " << gs->pos.line << ':' << gs->pos.col; log(m.str());
        if (!lines_.contains(gs->targetLine)) {
            if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing GOSUB target line " << gs->targetLine << " @ " << gs->pos.line << ':' << gs->pos.col; log(err.str()); throw SemanticError(err.str()); }
            else { std::ostringstream w; w << "Warning: GOSUB missing target line " << gs->targetLine << " @ " << gs->pos.line << ':' << gs->pos.col; log(w.str()); }
        }
        return;
    }
    if (dyn_cast<const ReturnStmt>(s)) { log("Return"); return; }
    if (dyn_cast<const EndStmt>(s)) { log("End"); return; }
    if (auto rz = dyn_cast<const RandomizeStmt>(s)) {
        log("Randomize");
        if (rz->seed) {
            if (typeOf(rz->seed.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: RANDOMIZE requires numeric seed @ " << rz->pos.line << ':' << rz->pos.col; log(m.str()); throw SemanticError(m.str()); }
            analyzeExpr(rz->seed.get());
        }
        return;
    }
}

} // namespace gwbasic

