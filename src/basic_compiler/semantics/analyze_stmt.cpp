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
#include "basic_compiler/ast/CloseStmt.h"
#include "basic_compiler/ast/DataStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/RestoreStmt.h"
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
#include "basic_compiler/ast/ColorStmt.h"
#include "basic_compiler/ast/ScreenStmt.h"
#include "basic_compiler/ast/CircleStmt.h"
#include "basic_compiler/ast/ClearStmt.h"
#include "basic_compiler/ast/EraseStmt.h"
#include "basic_compiler/ast/SwapStmt.h"
#include "basic_compiler/ast/OptionBaseStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
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
    if (auto ob = dyn_cast<const OptionBaseStmt>(s)) {
        if (!(ob->base == 0 || ob->base == 1)) { std::ostringstream m; m << "TypeError: OPTION BASE must be 0 or 1 @ " << ob->pos.line << ':' << ob->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        optionBase_ = ob->base;
        log() << "OptionBase=" << optionBase_ << '\n';
        return;
    }
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
    if (auto ma = dyn_cast<const MidAssignStmt>(s)) {
        // Target must be string variable or string array element
        if (!ma->indices.empty()) {
            // String array element
            if (!arrays_.contains(ma->name)) { std::ostringstream m; m << "TypeError: array '" << ma->name << "' not DIM'd @ " << ma->pos.line << ':' << ma->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            if (!varNameIsString(ma->name)) { std::ostringstream m; m << "TypeError: MID$ target array must be string '" << ma->name << "' @ " << ma->pos.line << ':' << ma->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            const auto& dims = arrays_.at(ma->name);
            if (ma->indices.size() != dims.size()) { std::ostringstream m; m << "ArityError: array '" << ma->name << "' expects " << dims.size() << " indices @ " << ma->pos.line << ':' << ma->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            for (const auto& idx : ma->indices) {
                if (typeOf(idx.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: MID$ index must be numeric @ " << ma->pos.line << ':' << ma->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                analyzeExpr(idx.get());
            }
        } else {
            reference(ma->name, ma->pos);
            if (!varNameIsString(ma->name)) {
                std::ostringstream m; m << "TypeError: MID$ target must be string variable '" << ma->name << "' @ " << ma->pos.line << ':' << ma->pos.col; log() << m.str() << '\n';
                throw SemanticError(m.str());
            }
        }
        // start and (optional) len must be numeric; value must be string
        if (typeOf(ma->start.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: MID$ start must be numeric @ " << ma->pos.line << ':' << ma->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (ma->len && typeOf(ma->len.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: MID$ length must be numeric @ " << ma->pos.line << ':' << ma->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (typeOf(ma->value.get()) != ValueType::String) { std::ostringstream m; m << "TypeError: MID$ assignment requires string value @ " << ma->pos.line << ':' << ma->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(ma->start.get()); if (ma->len) analyzeExpr(ma->len.get()); analyzeExpr(ma->value.get());
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
        if (d->upperBounds.empty()) { std::ostringstream m; m << "TypeError: DIM requires at least one bound @ " << d->pos.line << ':' << d->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        for (int ub : d->upperBounds) {
            if (ub < 0) { std::ostringstream m; m << "TypeError: DIM bounds must be non-negative @ " << d->pos.line << ':' << d->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        }
        arrays_[d->name] = d->upperBounds;
        allArrays_[d->name] = d->upperBounds; // record for codegen regardless of later ERASE
        std::ostringstream m; m << "Dim " << d->name << "(";
        for (size_t i = 0; i < d->upperBounds.size(); ++i) { if (i) m << ','; m << d->upperBounds[i]; }
        m << ")"; log() << m.str() << '\n';
        return;
    }
    if (auto aa = dyn_cast<const ArrayAssignStmt>(s)) {
        // Require array declared
        if (!arrays_.contains(aa->name)) { std::ostringstream m; m << "TypeError: array '" << aa->name << "' not DIM'd @ " << aa->pos.line << ':' << aa->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        const auto& dims = arrays_.at(aa->name);
        if (aa->indices.size() != dims.size()) { std::ostringstream m; m << "ArityError: array '" << aa->name << "' expects " << dims.size() << " indices @ " << aa->pos.line << ':' << aa->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        for (const auto& idx : aa->indices) {
            if (typeOf(idx.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: array index must be numeric @ " << aa->pos.line << ':' << aa->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
            analyzeExpr(idx.get());
        }
        // Type-specific checks: string arrays accept string values; numeric arrays accept numeric values
        const bool isStrArray = varNameIsString(aa->name);
        const auto vty = typeOf(aa->value.get());
        if (isStrArray && vty != ValueType::String) { std::ostringstream m; m << "TypeError: cannot assign number into string array @ " << aa->pos.line << ':' << aa->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (!isStrArray && vty == ValueType::String) { std::ostringstream m; m << "TypeError: cannot assign string into numeric array @ " << aa->pos.line << ':' << aa->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
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
            if (!t.indices.empty()) {
                if (!arrays_.contains(t.name)) { std::ostringstream m; m << "TypeError: array '" << t.name << "' not DIM'd @ " << rd->pos.line << ':' << rd->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                const auto& dims = arrays_.at(t.name);
                if (t.indices.size() != dims.size()) { std::ostringstream m; m << "ArityError: array '" << t.name << "' expects " << dims.size() << " indices @ " << rd->pos.line << ':' << rd->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                for (const auto& idx : t.indices) {
                    if (typeOf(idx.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: READ index must be numeric @ " << rd->pos.line << ':' << rd->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                    analyzeExpr(idx.get());
                }
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
    if (auto er = dyn_cast<const EraseStmt>(s)) {
        // ERASE: remove arrays from the current environment; subsequent uses require re-DIM
        for (const auto& n : er->names) {
            if (arrays_.contains(n)) {
                arrays_.erase(n);
            }
            std::ostringstream m; m << "Erase " << n << " @ " << er->pos.line << ':' << er->pos.col; log() << m.str() << '\n';
        }
        return;
    }
    if (auto sw = dyn_cast<const SwapStmt>(s)) {
        auto checkRef = [&](const ReadTarget& t, bool& isString) {
            if (t.indices.empty()) {
                // Scalar var
                reference(t.name, sw->pos);
                isString = varNameIsString(t.name);
            } else {
                // Array element
                if (!arrays_.contains(t.name)) { std::ostringstream m; m << "TypeError: array '" << t.name << "' not DIM'd @ " << sw->pos.line << ':' << sw->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                const auto& dims = arrays_.at(t.name);
                if (t.indices.size() != dims.size()) { std::ostringstream m; m << "ArityError: array '" << t.name << "' expects " << dims.size() << " indices @ " << sw->pos.line << ':' << sw->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                for (const auto& idx : t.indices) {
                    if (typeOf(idx.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: array index must be numeric @ " << sw->pos.line << ':' << sw->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
                    analyzeExpr(idx.get());
                }
                isString = varNameIsString(t.name);
            }
        };
        bool lstr = false, rstr = false;
        checkRef(sw->left, lstr);
        checkRef(sw->right, rstr);
        if (lstr != rstr) { std::ostringstream m; m << "TypeError: SWAP operands must be both numeric or both string @ " << sw->pos.line << ':' << sw->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        return;
    }
    if (auto bl = dyn_cast<const BloadStmt>(s)) {
        log() << "Warning: unsafe BLOAD at line " << bl->pos.line << '\n';
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
        log() << "Warning: unsafe BSAVE at line " << bs->pos.line << '\n';
        if (!dyn_cast<StringExpr>(bs->filename.get())) { std::ostringstream m; m << "TypeError: BSAVE filename must be string @ " << bs->pos.line << ':' << bs->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(bs->filename.get());
        if (typeOf(bs->offset.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: BSAVE offset must be numeric @ " << bs->pos.line << ':' << bs->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (typeOf(bs->length.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: BSAVE length must be numeric @ " << bs->pos.line << ':' << bs->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(bs->offset.get());
        analyzeExpr(bs->length.get());
        return;
    }
    if (auto pk = dyn_cast<const PokeStmt>(s)) {
        log() << "Warning: unsafe POKE at line " << pk->pos.line << '\n';
        if (typeOf(pk->address.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: POKE address must be numeric @ " << pk->pos.line << ':' << pk->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (typeOf(pk->value.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: POKE value must be numeric @ " << pk->pos.line << ':' << pk->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(pk->address.get());
        analyzeExpr(pk->value.get());
        return;
    }
    if (auto ca = dyn_cast<const CallAbsStmt>(s)) {
        log() << "Warning: unsafe CALL at line " << ca->pos.line << '\n';
        if (typeOf(ca->address.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CALL address must be numeric @ " << ca->pos.line << ':' << ca->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(ca->address.get());
        return;
    }
    if (auto du = dyn_cast<const DefUsrStmt>(s)) {
        log() << "Warning: unsafe DEF USR at line " << du->pos.line << '\n';
        if (typeOf(du->address.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: DEF USR address must be numeric @ " << du->pos.line << ':' << du->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(du->address.get());
        log() << "DefUsr" << '\n';
        return;
    }
    if (auto cd = dyn_cast<const ChdirStmt>(s)) {
        log() << "Warning: unsafe CHDIR at line " << cd->pos.line << '\n';
        if (typeOf(cd->path.get()) != ValueType::String) {
            std::ostringstream m; m << "TypeError: CHDIR requires string path @ " << cd->pos.line << ':' << cd->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        analyzeExpr(cd->path.get());
        return;
    }
    if (auto cs = dyn_cast<const ColorStmt>(s)) {
        if (cs->fg && typeOf(cs->fg.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: COLOR fg must be numeric @ " << cs->pos.line << ':' << cs->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (cs->bg && typeOf(cs->bg.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: COLOR bg must be numeric @ " << cs->pos.line << ':' << cs->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (cs->border && typeOf(cs->border.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: COLOR border must be numeric @ " << cs->pos.line << ':' << cs->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (cs->fg) analyzeExpr(cs->fg.get());
        if (cs->bg) analyzeExpr(cs->bg.get());
        if (cs->border) analyzeExpr(cs->border.get());
        return;
    }
    if (auto sc = dyn_cast<const ScreenStmt>(s)) {
        // All provided arguments must be numeric expressions if present
        if (sc->mode && typeOf(sc->mode.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: SCREEN mode must be numeric @ " << sc->pos.line << ':' << sc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (sc->colorSwitch && typeOf(sc->colorSwitch.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: SCREEN colorswitch must be numeric @ " << sc->pos.line << ':' << sc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (sc->aPage && typeOf(sc->aPage.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: SCREEN apage must be numeric @ " << sc->pos.line << ':' << sc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (sc->vPage && typeOf(sc->vPage.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: SCREEN vpage must be numeric @ " << sc->pos.line << ':' << sc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (sc->mode) analyzeExpr(sc->mode.get());
        if (sc->colorSwitch) analyzeExpr(sc->colorSwitch.get());
        if (sc->aPage) analyzeExpr(sc->aPage.get());
        if (sc->vPage) analyzeExpr(sc->vPage.get());
        log() << "Screen" << '\n';
        return;
    }
    if (auto cc = dyn_cast<const CircleStmt>(s)) {
        if (typeOf(cc->x.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE x must be numeric @ " << cc->pos.line << ':' << cc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (typeOf(cc->y.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE y must be numeric @ " << cc->pos.line << ':' << cc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (typeOf(cc->r.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE r must be numeric @ " << cc->pos.line << ':' << cc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (cc->color && typeOf(cc->color.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE color must be numeric @ " << cc->pos.line << ':' << cc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (cc->start && typeOf(cc->start.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE start must be numeric @ " << cc->pos.line << ':' << cc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (cc->end && typeOf(cc->end.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE end must be numeric @ " << cc->pos.line << ':' << cc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        if (cc->aspect && typeOf(cc->aspect.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE aspect must be numeric @ " << cc->pos.line << ':' << cc->pos.col; log() << m.str() << '\n'; throw SemanticError(m.str()); }
        analyzeExpr(cc->x.get());
        analyzeExpr(cc->y.get());
        analyzeExpr(cc->r.get());
        if (cc->color) analyzeExpr(cc->color.get());
        if (cc->start) analyzeExpr(cc->start.get());
        if (cc->end) analyzeExpr(cc->end.get());
        if (cc->aspect) analyzeExpr(cc->aspect.get());
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
    if (dyn_cast<const ClearStmt>(s)) { log() << "Clear" << '\n'; return; }
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
    if (auto og = dyn_cast<const OnGotoStmt>(s)) {
        if (typeOf(og->index.get()) == ValueType::String) {
            std::ostringstream m; m << "TypeError: ON index cannot be string @ " << og->pos.line << ':' << og->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        analyzeExpr(og->index.get());
        for (int ln : og->targets) {
            if (!lines_.contains(ln)) {
                if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing ON GOTO target line " << ln << " @ " << og->pos.line << ':' << og->pos.col; log() << err.str() << '\n'; throw SemanticError(err.str()); }
                else { std::ostringstream w; w << "Warning: ON GOTO missing target line " << ln << " @ " << og->pos.line << ':' << og->pos.col; log() << w.str() << '\n'; }
            }
        }
        return;
    }
    if (auto ogs = dyn_cast<const OnGosubStmt>(s)) {
        if (typeOf(ogs->index.get()) == ValueType::String) {
            std::ostringstream m; m << "TypeError: ON index cannot be string @ " << ogs->pos.line << ':' << ogs->pos.col; log() << m.str() << '\n';
            throw SemanticError(m.str());
        }
        analyzeExpr(ogs->index.get());
        for (int ln : ogs->targets) {
            if (!lines_.contains(ln)) {
                if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing ON GOSUB target line " << ln << " @ " << ogs->pos.line << ':' << ogs->pos.col; log() << err.str() << '\n'; throw SemanticError(err.str()); }
                else { std::ostringstream w; w << "Warning: ON GOSUB missing target line " << ln << " @ " << ogs->pos.line << ':' << ogs->pos.col; log() << w.str() << '\n'; }
            }
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
