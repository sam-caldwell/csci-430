// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/CommonStmt.h"
#include "basic_compiler/ast/ChainStmt.h"
#include "basic_compiler/ast/DimStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/OpenStmt.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/DefFnStmt.h"
#include "basic_compiler/ast/DefTypeStmt.h"
#include "basic_compiler/ast/DefSegStmt.h"
#include "basic_compiler/ast/BloadStmt.h"
#include "basic_compiler/ast/BsaveStmt.h"
#include "basic_compiler/ast/PokeStmt.h"
#include "basic_compiler/ast/CallAbsStmt.h"
#include "basic_compiler/ast/DefUsrStmt.h"
#include "basic_compiler/ast/ChdirStmt.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::analyzeStmt
 * Inputs:
 *  - s: Statement node to analyze
 * Outputs:
 *  - void (updates internal state, may throw on semantic errors)
 * Theory of operation:
 *  - Dispatches by statement kind performing type checks, control-flow
 *    validation, scope handling, and logs relevant events.
 */
void SemanticAnalyzer::analyzeStmt(const Stmt* s) {
    if (auto p = dyn_cast<const PrintStmt>(s)) {
        if (p->value) analyzeExpr(p->value.get());
        for (const auto& v : p->more) analyzeExpr(v.get());
        return;
    }
    if (auto a = dyn_cast<const AssignStmt>(s)) {
        reference(a->name, a->pos);
        auto vt = typeOf(a->value.get());
        const bool varIsString = varNameIsString(a->name);
        if (varIsString && vt != ValueType::String) {
            std::ostringstream m; m << "TypeError: cannot assign number to string var '" << a->name << "' @ " << a->pos.line << ':' << a->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        if (!varIsString && vt == ValueType::String) {
            std::ostringstream m; m << "TypeError: cannot assign string to numeric var '" << a->name << "' @ " << a->pos.line << ':' << a->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        analyzeExpr(a->value.get());
        return;
    }
    if (auto i = dyn_cast<const IfStmt>(s)) {
        if (typeOf(i->cond.get()) == ValueType::String) {
            std::ostringstream m; m << "TypeError: IF condition cannot be string @ " << i->pos.line << ':' << i->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        analyzeExpr(i->cond.get());
        if (!lines_.contains(i->targetLine)) {
            if (strictControlFlow_) {
                std::ostringstream err; err << "ControlFlowError: missing IF target line " << i->targetLine << " @ " << i->pos.line << ':' << i->pos.col; log() << err.str() << '\n';
                throw SemanticError(err.str());
            } else {
                std::ostringstream w; w << "Warning: IF missing target line " << i->targetLine << " @ " << i->pos.line << ':' << i->pos.col; log() << w.str() << '\n';
            }
        }
        std::ostringstream m; m << "IfThen target=" << i->targetLine << " @ " << i->pos.line << ':' << i->pos.col; log() << m.str() << '\n';
        return;
    }
    if (auto d = dyn_cast<const DimStmt>(s)) {
        declare(d->name);
        if (d->length <= 0) { std::ostringstream m; m << "TypeError: DIM length must be positive @ " << d->pos.line << ':' << d->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        arrays_[d->name] = d->length;
        std::ostringstream m; m << "Dim " << d->name << "(" << d->length << ")"; log() << m.str() << '\n';
        return;
    }
    if (auto aa = dyn_cast<const ArrayAssignStmt>(s)) {
        // Require array declared
        if (!arrays_.contains(aa->name)) { std::ostringstream m; m << "TypeError: array '" << aa->name << "' not DIM'd @ " << aa->pos.line << ':' << aa->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (typeOf(aa->index.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: array index must be numeric @ " << aa->pos.line << ':' << aa->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (typeOf(aa->value.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: cannot assign string into numeric array @ " << aa->pos.line << ':' << aa->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(aa->index.get());
        analyzeExpr(aa->value.get());
        return;
    }
    if (auto op = dyn_cast<const OpenStmt>(s)) {
        // Minimal validation of channel
        if (op->channel < 1 || op->channel > 16) { std::ostringstream m; m << "IOError: channel out of range @ " << op->pos.line << ':' << op->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(op->filename.get());
        return;
    }
    if (auto cl = dyn_cast<const CloseStmt>(s)) {
        if (cl->channel < 1 || cl->channel > 16) { std::ostringstream m; m << "IOError: channel out of range @ " << cl->pos.line << ':' << cl->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        return;
    }
    if (auto d = dyn_cast<const DataStmt>(s)) {
        std::ostringstream m; m << "Data count=" << d->items.size(); log() << m.str() << '\n';
        return;
    }
    if (auto rd = dyn_cast<const ReadStmt>(s)) {
        for (const auto& t : rd->targets) {
            if (t.index) {
                if (!arrays_.contains(t.name)) { std::ostringstream m; m << "TypeError: array '" << t.name << "' not DIM'd @ " << rd->pos.line << ':' << rd->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                if (typeOf(t.index.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: READ index must be numeric @ " << rd->pos.line << ':' << rd->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                analyzeExpr(t.index.get());
            } else {
                reference(t.name, rd->pos);
            }
        }
        return;
    }
    if (dyn_cast<const RestoreStmt>(s)) { log() << "Restore" << '\n'; return; }
    if (auto wr = dyn_cast<const WriteStmt>(s)) { for (const auto& e : wr->items) analyzeExpr(e.get()); return; }
    if (auto ib = dyn_cast<const IfBlockStmt>(s)) {
        if (typeOf(ib->cond.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: IF condition cannot be string @ " << ib->pos.line << ':' << ib->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(ib->cond.get());
        enterScope();
        for (const auto& st : ib->thenBody) analyzeStmt(st.get());
        exitScope();
        if (!ib->elseBody.empty()) {
            enterScope();
            for (const auto& st : ib->elseBody) analyzeStmt(st.get());
            exitScope();
        }
        return;
    }
    if (auto f = dyn_cast<const ForStmt>(s)) {
        reference(f->var, f->pos);
        std::ostringstream m; m << "For var=" << f->var << " @ " << f->pos.line << ':' << f->pos.col; log() << m.str() << '\n';
        if (typeOf(f->start.get()) == ValueType::String) { std::ostringstream err; err << "TypeError: FOR start must be numeric @ " << f->pos.line << ':' << f->pos.col; log() << err.str() << '\n'; throw SemanticError(err.str()); }
        if (typeOf(f->end.get()) == ValueType::String) { std::ostringstream err; err << "TypeError: FOR end must be numeric @ " << f->pos.line << ':' << f->pos.col; log() << err.str() << '\n'; throw SemanticError(err.str()); }
        if (f->step && typeOf(f->step.get()) == ValueType::String) { std::ostringstream err; err << "TypeError: FOR step must be numeric @ " << f->pos.line << ':' << f->pos.col; log() << err.str() << '\n'; throw SemanticError(err.str()); }
        analyzeExpr(f->start.get());
        analyzeExpr(f->end.get());
        analyzeExpr(f->step.get());
        enterScope();
        for (const auto& bs : f->body) analyzeStmt(bs.get());
        exitScope();
        return;
    }
    if (auto in = dyn_cast<const InputStmt>(s)) { reference(in->name, in->pos); return; }
    if (auto dt = dyn_cast<const DefTypeStmt>(s)) {
        for (const auto& [a,b] : dt->ranges) {
            for (char ch = a; ch <= b; ++ch) {
                int idx = (std::toupper(static_cast<unsigned char>(ch)) - 'A');
                if (idx >= 0 && idx < 26) {
                    switch (dt->kind) {
                        case DefTypeStmt::Kind::Int: defaultKinds_[idx] = DefaultKind::Int; break;
                        case DefTypeStmt::Kind::Sng: defaultKinds_[idx] = DefaultKind::Sng; break;
                        case DefTypeStmt::Kind::Dbl: defaultKinds_[idx] = DefaultKind::Dbl; break;
                        case DefTypeStmt::Kind::Str: defaultKinds_[idx] = DefaultKind::Str; break;
                    }
                }
            }
        }
        log() << "DefType" << '\n';
        return;
    }
    if (auto ds = dyn_cast<const DefSegStmt>(s)) {
        if (ds->value) {
            if (typeOf(ds->value.get()) == ValueType::String) {
                std::ostringstream m; m << "TypeError: DEF SEG requires numeric segment @ " << ds->pos.line << ':' << ds->pos.col; log() << m.str() << '\n';
                throw SemanticError(m.str());
            }
            analyzeExpr(ds->value.get());
        }
        log() << "DefSeg" << '\n';
        return;
    }
    if (auto bl = dyn_cast<const BloadStmt>(s)) {
        if (!dyn_cast<StringExpr>(bl->filename.get())) {
            std::ostringstream m; m << "TypeError: BLOAD filename must be string @ " << bl->pos.line << ':' << bl->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        analyzeExpr(bl->filename.get());
        if (bl->offset) {
            if (typeOf(bl->offset.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: BLOAD offset must be numeric @ " << bl->pos.line << ':' << bl->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            analyzeExpr(bl->offset.get());
        }
        return;
    }
    if (auto bs = dyn_cast<const BsaveStmt>(s)) {
        if (!dyn_cast<StringExpr>(bs->filename.get())) { std::ostringstream m; m << "TypeError: BSAVE filename must be string @ " << bs->pos.line << ':' << bs->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(bs->filename.get());
        if (typeOf(bs->offset.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: BSAVE offset must be numeric @ " << bs->pos.line << ':' << bs->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (typeOf(bs->length.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: BSAVE length must be numeric @ " << bs->pos.line << ':' << bs->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(bs->offset.get());
        analyzeExpr(bs->length.get());
        return;
    }
    if (auto pk = dyn_cast<const PokeStmt>(s)) {
        if (typeOf(pk->address.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: POKE address must be numeric @ " << pk->pos.line << ':' << pk->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (typeOf(pk->value.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: POKE value must be numeric @ " << pk->pos.line << ':' << pk->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(pk->address.get());
        analyzeExpr(pk->value.get());
        return;
    }
    if (auto ca = dyn_cast<const CallAbsStmt>(s)) {
        if (typeOf(ca->address.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CALL address must be numeric @ " << ca->pos.line << ':' << ca->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(ca->address.get());
        return;
    }
    if (auto du = dyn_cast<const DefUsrStmt>(s)) {
        if (typeOf(du->address.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: DEF USR address must be numeric @ " << du->pos.line << ':' << du->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(du->address.get());
        log() << "DefUsr" << '\n';
        return;
    }
    if (auto cd = dyn_cast<const ChdirStmt>(s)) {
        if (typeOf(cd->path.get()) != ValueType::String) {
            std::ostringstream m; m << "TypeError: CHDIR requires string path @ " << cd->pos.line << ':' << cd->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        analyzeExpr(cd->path.get());
        return;
    }
    if (auto df = dyn_cast<const DefFnStmt>(s)) {
        // Record user-defined function; body must be a valid expression.
        // Normalize function name to uppercase for lookup.
        std::string up = df->fnName; for (auto &ch: up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if (userFunctions_.contains(up)) {
            std::ostringstream m; m << "SemanticError: duplicate DEF for function '" << df->fnName << "' @ " << df->pos.line << ':' << df->pos.col;
            log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        // Analyze body with param considered local; suppress global reference tracking for it
        currentFnParam_ = df->paramName;
        // Validate return type vs. body type
        const bool retIsString = (!df->fnName.empty() && df->fnName.back() == '$');
        ValueType bt = typeOf(df->body.get());
        if (retIsString && bt != ValueType::String) {
            std::ostringstream m; m << "TypeError: DEF " << df->fnName << " must return string; got numeric @ " << df->pos.line << ':' << df->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        if (!retIsString && bt == ValueType::String) {
            std::ostringstream m; m << "TypeError: DEF " << df->fnName << " must return number; got string @ " << df->pos.line << ':' << df->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        analyzeExpr(df->body.get());
        currentFnParam_.reset();
        userFunctions_[up] = df;
        log() << "DefFn " << df->fnName << '\n';
        return;
    }
    if (auto g = dyn_cast<const GotoStmt>(s)) {
        std::ostringstream m; m << "Goto target=" << g->targetLine << " @ " << g->pos.line << ':' << g->pos.col; log() << m.str() << '\n';
        if (!lines_.contains(g->targetLine)) {
            if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing GOTO target line " << g->targetLine << " @ " << g->pos.line << ':' << g->pos.col; log() << err.str() << '\n'; throw SemanticError(err.str()); }
            else { std::ostringstream w; w << "Warning: GOTO missing target line " << g->targetLine << " @ " << g->pos.line << ':' << g->pos.col; log() << w.str() << '\n'; }
        }
        return;
    }
    if (auto gs = dyn_cast<const GosubStmt>(s)) {
        std::ostringstream m; m << "Gosub target=" << gs->targetLine << " @ " << gs->pos.line << ':' << gs->pos.col; log() << m.str() << '\n';
        if (!lines_.contains(gs->targetLine)) {
            if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing GOSUB target line " << gs->targetLine << " @ " << gs->pos.line << ':' << gs->pos.col; log() << err.str() << '\n'; throw SemanticError(err.str()); }
            else { std::ostringstream w; w << "Warning: GOSUB missing target line " << gs->targetLine << " @ " << gs->pos.line << ':' << gs->pos.col; log() << w.str() << '\n'; }
        }
        return;
    }
    if (dyn_cast<const ReturnStmt>(s)) { log() << "Return" << '\n'; return; }
    if (dyn_cast<const EndStmt>(s)) { log() << "End" << '\n'; return; }
    if (auto rz = dyn_cast<const RandomizeStmt>(s)) {
        log() << "Randomize" << '\n';
        if (rz->seed) {
            if (typeOf(rz->seed.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: RANDOMIZE requires numeric seed @ " << rz->pos.line << ':' << rz->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            analyzeExpr(rz->seed.get());
        }
        return;
    }
    if (auto w = dyn_cast<const WhileStmt>(s)) {
        if (typeOf(w->cond.get()) == ValueType::String) {
            std::ostringstream m; m << "TypeError: WHILE condition cannot be string @ " << w->pos.line << ':' << w->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        analyzeExpr(w->cond.get());
        enterScope();
        for (const auto& bs : w->body) analyzeStmt(bs.get());
        exitScope();
        return;
    }
    if (auto c = dyn_cast<const CommonStmt>(s)) {
        for (const auto& n : c->names) {
            declare(n);
            common_.insert(n);
            std::ostringstream m; m << "Common " << n << " @ " << c->pos.line << ':' << c->pos.col; log() << m.str() << '\n';
        }
        return;
    }
    if (auto ch = dyn_cast<const ChainStmt>(s)) {
        if (ch->targetLine.has_value() && !lines_.contains(*ch->targetLine)) {
            if (strictControlFlow_) {
                std::ostringstream err; err << "ControlFlowError: missing CHAIN target line " << *ch->targetLine << " @ " << ch->pos.line << ':' << ch->pos.col; log() << err.str() << '\n';
                throw SemanticError(err.str());
            } else {
                std::ostringstream w; w << "Warning: CHAIN missing target line " << *ch->targetLine << " @ " << ch->pos.line << ':' << ch->pos.col; log() << w.str() << '\n';
            }
        }
        log() << "Chain" << '\n';
        return;
    }
}

} // namespace gwbasic
