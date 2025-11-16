// (c) 2025 Sam Caldwell. All Rights Reserved.
// NOLINTBEGIN(llvm-include-order,misc-include-cleaner)
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/semantics/SemanticError.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
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
#include "basic_compiler/ast/LineInputStmt.h"
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
#include "basic_compiler/ast/OptionPrintZonesStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/UnsupportedStmt.h"
#include "basic_compiler/ast/ClsStmt.h"
#include "basic_compiler/ast/LocateStmt.h"
#include "basic_compiler/ast/WidthStmt.h"
#include "basic_compiler/ast/FilesStmt.h"
#include "basic_compiler/ast/MkdirStmt.h"
#include "basic_compiler/ast/RmdirStmt.h"
#include "basic_compiler/ast/KillStmt.h"
#include "basic_compiler/ast/NameStmt.h"
#include "basic_compiler/ast/ShellStmt.h"
#include "basic_compiler/ast/EnvironStmt.h"
#include "basic_compiler/ast/BeepStmt.h"
#include <sstream>
#include <string>
// NOLINTEND(llvm-include-order,misc-include-cleaner)
// NOLINTBEGIN(readability-function-cognitive-complexity,readability-function-size,readability-identifier-length,readability-braces-around-statements,readability-qualified-auto,llvm-qualified-auto,readability-implicit-bool-conversion,readability-simplify-boolean-expr,llvm-prefer-isa-or-dyn-cast-in-conditionals,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

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
    constexpr int kMaxChannel = 16; // NOLINT(readability-magic-numbers)
    if (auto us = dyn_cast<const UnsupportedStmt>(s)) {
        // For now, unsupported statements are accepted but logged as no-ops.
        
        return;
    }
    if (isa<const ClsStmt>(s)) { return; }
    if (auto lc = dyn_cast<const LocateStmt>(s)) {
        if (typeOf(lc->row.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: LOCATE row must be numeric @ " << lc->pos.line << ':' << lc->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(lc->row.get());
        if (lc->col) {
            if (typeOf(lc->col.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: LOCATE col must be numeric @ " << lc->pos.line << ':' << lc->pos.col; throw SemanticError(m.str()); }
            analyzeExpr(lc->col.get());
        }
        
        return;
    }
    if (auto wd = dyn_cast<const WidthStmt>(s)) {
        // WIDTH n  OR  WIDTH dev$, n
        if (wd->device) {
            if (typeOf(wd->device.get()) != ValueType::String) {
                std::ostringstream m; m << "TypeError: WIDTH device must be string @ " << wd->pos.line << ':' << wd->pos.col; throw SemanticError(m.str());
            }
            analyzeExpr(wd->device.get());
        }
        if (typeOf(wd->columns.get()) == ValueType::String) {
                std::ostringstream m; m << "TypeError: WIDTH columns must be numeric @ " << wd->pos.line << ':' << wd->pos.col; throw SemanticError(m.str());
        }
        analyzeExpr(wd->columns.get());
        
        return;
    }
    if (auto fl = dyn_cast<const FilesStmt>(s)) {
        if (fl->device) {
            if (typeOf(fl->device.get()) != ValueType::String) {
                std::ostringstream m; m << "TypeError: FILES device must be string @ " << fl->pos.line << ':' << fl->pos.col; throw SemanticError(m.str());
            }
            analyzeExpr(fl->device.get());
        }
        if (fl->pattern) {
            if (typeOf(fl->pattern.get()) != ValueType::String) {
                std::ostringstream m; m << "TypeError: FILES path/pattern must be string @ " << fl->pos.line << ':' << fl->pos.col; throw SemanticError(m.str());
            }
            analyzeExpr(fl->pattern.get());
        }
        
        return;
    }
    if (auto mk = dyn_cast<const MkdirStmt>(s)) {
        if (typeOf(mk->path.get()) != ValueType::String) {
            std::ostringstream m; m << "TypeError: MKDIR requires string path @ " << mk->pos.line << ':' << mk->pos.col; throw SemanticError(m.str());
        }
        analyzeExpr(mk->path.get());
        
        return;
    }
    if (auto rd = dyn_cast<const RmdirStmt>(s)) {
        if (typeOf(rd->path.get()) != ValueType::String) {
            std::ostringstream m; m << "TypeError: RMDIR requires string path @ " << rd->pos.line << ':' << rd->pos.col; throw SemanticError(m.str());
        }
        analyzeExpr(rd->path.get());
        
        return;
    }
    if (auto kl = dyn_cast<const KillStmt>(s)) {
        if (typeOf(kl->filespec.get()) != ValueType::String) {
            std::ostringstream m; m << "TypeError: KILL requires string filespec @ " << kl->pos.line << ':' << kl->pos.col; throw SemanticError(m.str());
        }
        analyzeExpr(kl->filespec.get());
        
        return;
    }
    if (auto nm = dyn_cast<const NameStmt>(s)) {
        if (typeOf(nm->oldName.get()) != ValueType::String || typeOf(nm->newName.get()) != ValueType::String) {
            std::ostringstream m; m << "TypeError: NAME requires string operands @ " << nm->pos.line << ':' << nm->pos.col; throw SemanticError(m.str());
        }
        analyzeExpr(nm->oldName.get());
        analyzeExpr(nm->newName.get());
        
        return;
    }
    if (auto sh = dyn_cast<const ShellStmt>(s)) {
        if (sh->command && typeOf(sh->command.get()) != ValueType::String) {
            std::ostringstream m; m << "TypeError: SHELL command must be string @ " << sh->pos.line << ':' << sh->pos.col; throw SemanticError(m.str());
        }
        if (sh->command) analyzeExpr(sh->command.get());
        
        return;
    }
    if (auto ev = dyn_cast<const EnvironStmt>(s)) {
        if (typeOf(ev->spec.get()) != ValueType::String) { std::ostringstream m; m << "TypeError: ENVIRON requires string spec @ " << ev->pos.line << ':' << ev->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(ev->spec.get());
        
        return;
    }
    if (dyn_cast<const BeepStmt>(s)) { return; }
    if (auto ob = dyn_cast<const OptionBaseStmt>(s)) {
        if (!(ob->base == 0 || ob->base == 1)) { std::ostringstream m; m << "TypeError: OPTION BASE must be 0 or 1 @ " << ob->pos.line << ':' << ob->pos.col; throw SemanticError(m.str()); }
        optionBase_ = ob->base;
        
        return;
    }
    if (auto opz = dyn_cast<const OptionPrintZonesStmt>(s)) {
        printZones_ = opz->enabled;
        return;
    }
    if (auto p = dyn_cast<const PrintStmt>(s)) {
        // Enforce PRINT USING requires string format when present
        if (p->format) {
            if (typeOf(p->format.get()) != ValueType::String) { std::ostringstream m; m << "TypeError: PRINT USING requires string format @ " << p->pos.line << ':' << p->pos.col; throw SemanticError(m.str()); }
            analyzeExpr(p->format.get());
        }
        if (p->value) analyzeExpr(p->value.get());
        for (const auto& v : p->more) analyzeExpr(v.get());
        return;
    }
    if (auto a = dyn_cast<const AssignStmt>(s)) {
        reference(a->name, a->pos);
        auto vt = typeOf(a->value.get());
        const bool varIsString = varNameIsString(a->name);
        if (varIsString && vt != ValueType::String) { std::ostringstream m; m << "TypeError: cannot assign number to string var '" << a->name << "' @ " << a->pos.line << ':' << a->pos.col; throw SemanticError(m.str()); }
        if (!varIsString && vt == ValueType::String) { std::ostringstream m; m << "TypeError: cannot assign string to numeric var '" << a->name << "' @ " << a->pos.line << ':' << a->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(a->value.get());
        return;
    }
    if (auto ma = dyn_cast<const MidAssignStmt>(s)) {
        // Target must be string variable or string array element
        if (!ma->indices.empty()) {
            // String array element
            if (!arrays_.contains(ma->name)) { std::ostringstream m; m << "TypeError: array '" << ma->name << "' not DIM'd @ " << ma->pos.line << ':' << ma->pos.col; throw SemanticError(m.str()); }
            if (!varNameIsString(ma->name)) { std::ostringstream m; m << "TypeError: MID$ target array must be string '" << ma->name << "' @ " << ma->pos.line << ':' << ma->pos.col; throw SemanticError(m.str()); }
            const auto& dims = arrays_.at(ma->name);
            if (ma->indices.size() != dims.size()) { std::ostringstream m; m << "ArityError: array '" << ma->name << "' expects " << dims.size() << " indices @ " << ma->pos.line << ':' << ma->pos.col; throw SemanticError(m.str()); }
            for (const auto& idx : ma->indices) {
                if (typeOf(idx.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: MID$ index must be numeric @ " << ma->pos.line << ':' << ma->pos.col; throw SemanticError(m.str()); }
                analyzeExpr(idx.get());
            }
        } else {
            reference(ma->name, ma->pos);
            if (!varNameIsString(ma->name)) { std::ostringstream m; m << "TypeError: MID$ target must be string variable '" << ma->name << "' @ " << ma->pos.line << ':' << ma->pos.col; throw SemanticError(m.str()); }
        }
        // start and (optional) len must be numeric; value must be string
        if (typeOf(ma->start.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: MID$ start must be numeric @ " << ma->pos.line << ':' << ma->pos.col; throw SemanticError(m.str()); }
        if (ma->len && typeOf(ma->len.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: MID$ length must be numeric @ " << ma->pos.line << ':' << ma->pos.col; throw SemanticError(m.str()); }
        if (typeOf(ma->value.get()) != ValueType::String) { std::ostringstream m; m << "TypeError: MID$ assignment requires string value @ " << ma->pos.line << ':' << ma->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(ma->start.get()); if (ma->len) analyzeExpr(ma->len.get()); analyzeExpr(ma->value.get());
        return;
    }
    if (auto i = dyn_cast<const IfStmt>(s)) {
        if (typeOf(i->cond.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: IF condition cannot be string @ " << i->pos.line << ':' << i->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(i->cond.get());
        if (!lines_.contains(i->targetLine)) {
            if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing IF target line " << i->targetLine << " @ " << i->pos.line << ':' << i->pos.col; throw SemanticError(err.str()); }
        }
        
        return;
    }
    if (auto d = dyn_cast<const DimStmt>(s)) {
        declare(d->name);
        if (d->upperBounds.empty()) { std::ostringstream m; m << "TypeError: DIM requires at least one bound @ " << d->pos.line << ':' << d->pos.col; throw SemanticError(m.str()); }
        for (const int ub : d->upperBounds) {
            if (ub < 0) { std::ostringstream m; m << "TypeError: DIM bounds must be non-negative @ " << d->pos.line << ':' << d->pos.col; throw SemanticError(m.str()); }
        }
        arrays_[d->name] = d->upperBounds;
        allArrays_[d->name] = d->upperBounds; // record for codegen regardless of later ERASE
        std::ostringstream m; m << "Dim " << d->name << "(";
        for (size_t i = 0; i < d->upperBounds.size(); ++i) { // NOLINT(misc-include-cleaner)
            if (i != 0U) { m << ','; }
            m << d->upperBounds[i];
        }
        m << ")";
        return;
    }
    if (auto aa = dyn_cast<const ArrayAssignStmt>(s)) {
        // Require array declared
        if (!arrays_.contains(aa->name)) { std::ostringstream m; m << "TypeError: array '" << aa->name << "' not DIM'd @ " << aa->pos.line << ':' << aa->pos.col; throw SemanticError(m.str()); }
        const auto& dims = arrays_.at(aa->name);
        if (aa->indices.size() != dims.size()) { std::ostringstream m; m << "ArityError: array '" << aa->name << "' expects " << dims.size() << " indices @ " << aa->pos.line << ':' << aa->pos.col; throw SemanticError(m.str()); }
        for (const auto& idx : aa->indices) {
            if (typeOf(idx.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: array index must be numeric @ " << aa->pos.line << ':' << aa->pos.col; throw SemanticError(m.str()); }
            analyzeExpr(idx.get());
        }
        // Type-specific checks: string arrays accept string values; numeric arrays accept numeric values
        const bool isStrArray = varNameIsString(aa->name);
        const auto vty = typeOf(aa->value.get());
        if (isStrArray && vty != ValueType::String) { std::ostringstream m; m << "TypeError: cannot assign number into string array @ " << aa->pos.line << ':' << aa->pos.col; throw SemanticError(m.str()); }
        if (!isStrArray && vty == ValueType::String) { std::ostringstream m; m << "TypeError: cannot assign string into numeric array @ " << aa->pos.line << ':' << aa->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(aa->value.get());
        return;
    }
    if (auto op = dyn_cast<const OpenStmt>(s)) {
        // Minimal validation of channel
        constexpr int kMaxChannel = 16; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
        if (op->channel < 1 || op->channel > kMaxChannel) { std::ostringstream m; m << "IOError: channel out of range @ " << op->pos.line << ':' << op->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(op->filename.get());
        return;
    }
    if (auto cl = dyn_cast<const CloseStmt>(s)) {
        if (cl->channel < 1 || cl->channel > kMaxChannel) { std::ostringstream m; m << "IOError: channel out of range @ " << cl->pos.line << ':' << cl->pos.col; throw SemanticError(m.str()); }
        return;
    }
    if (auto d = dyn_cast<const DataStmt>(s)) {
        
        return;
    }
    if (auto rd = dyn_cast<const ReadStmt>(s)) {
        for (const auto& t : rd->targets) {
            if (!t.indices.empty()) {
                if (!arrays_.contains(t.name)) { std::ostringstream m; m << "TypeError: array '" << t.name << "' not DIM'd @ " << rd->pos.line << ':' << rd->pos.col; throw SemanticError(m.str()); }
                const auto& dims = arrays_.at(t.name);
                if (t.indices.size() != dims.size()) { std::ostringstream m; m << "ArityError: array '" << t.name << "' expects " << dims.size() << " indices @ " << rd->pos.line << ':' << rd->pos.col; throw SemanticError(m.str()); }
                for (const auto& idx : t.indices) {
                    if (typeOf(idx.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: READ index must be numeric @ " << rd->pos.line << ':' << rd->pos.col; throw SemanticError(m.str()); }
                    analyzeExpr(idx.get());
                }
            } else {
                reference(t.name, rd->pos);
            }
        }
        return;
    }
    if (dyn_cast<const RestoreStmt>(s)) { return; }
    if (auto wr = dyn_cast<const WriteStmt>(s)) { for (const auto& e : wr->items) { analyzeExpr(e.get()); } return; }
    if (auto ib = dyn_cast<const IfBlockStmt>(s)) {
        if (typeOf(ib->cond.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: IF condition cannot be string @ " << ib->pos.line << ':' << ib->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(ib->cond.get());
        enterScope();
        for (const auto& st : ib->thenBody) { analyzeStmt(st.get()); }
        exitScope();
        if (!ib->elseBody.empty()) {
            enterScope();
            for (const auto& st : ib->elseBody) { analyzeStmt(st.get()); }
            exitScope();
        }
        return;
    }
    if (auto f = dyn_cast<const ForStmt>(s)) {
        reference(f->var, f->pos);
        if (typeOf(f->start.get()) == ValueType::String) { std::ostringstream err; err << "TypeError: FOR start must be numeric @ " << f->pos.line << ':' << f->pos.col; throw SemanticError(err.str()); }
        if (typeOf(f->end.get()) == ValueType::String) { std::ostringstream err; err << "TypeError: FOR end must be numeric @ " << f->pos.line << ':' << f->pos.col; throw SemanticError(err.str()); }
        if (f->step && typeOf(f->step.get()) == ValueType::String) { std::ostringstream err; err << "TypeError: FOR step must be numeric @ " << f->pos.line << ':' << f->pos.col; throw SemanticError(err.str()); }
        analyzeExpr(f->start.get());
        analyzeExpr(f->end.get());
        analyzeExpr(f->step.get());
        enterScope();
        for (const auto& bs : f->body) { analyzeStmt(bs.get()); }
        exitScope();
        return;
    }
    if (auto in = dyn_cast<const InputStmt>(s)) {
        // Console INPUT only accepts numeric variables. Suggest LINE INPUT for strings.
        for (const auto& v : in->variables) {
            if (varNameIsString(v)) {
                std::ostringstream m; m << "TypeError: INPUT requires numeric variable; use LINE INPUT for strings @ "
                                        << in->pos.line << ':' << in->pos.col;
                throw SemanticError(m.str());
            }
            reference(v, in->pos);
        }
        if (in->promptLiteral) strings_.insert(*in->promptLiteral);
        return;
    }
    if (auto dt = dyn_cast<const DefTypeStmt>(s)) {
        for (const auto& [a,b] : dt->ranges) {
            for (char ch = a; ch <= b; ++ch) {
                const int idx = (std::toupper(static_cast<unsigned char>(ch)) - 'A'); // NOLINT(misc-include-cleaner)
                if (idx >= 0 && idx < 26) {
                    switch (dt->kind) {
                        case DefTypeStmt::Kind::Int: defaultKinds_.at(idx) = DefaultKind::Int; break; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
                        case DefTypeStmt::Kind::Sng: defaultKinds_.at(idx) = DefaultKind::Sng; break;
                        case DefTypeStmt::Kind::Dbl: defaultKinds_.at(idx) = DefaultKind::Dbl; break;
                        case DefTypeStmt::Kind::Str: defaultKinds_.at(idx) = DefaultKind::Str; break;
                    }
                }
            }
        }
        
        return;
    }
    if (auto ds = dyn_cast<const DefSegStmt>(s)) {
        if (ds->value) {
            if (typeOf(ds->value.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: DEF SEG requires numeric segment @ " << ds->pos.line << ':' << ds->pos.col; throw SemanticError(m.str()); }
            analyzeExpr(ds->value.get());
        }
        
        return;
    }
    if (auto er = dyn_cast<const EraseStmt>(s)) {
        // ERASE: remove arrays from the current environment; subsequent uses require re-DIM
        for (const auto& n : er->names) {
            if (arrays_.contains(n)) {
                arrays_.erase(n);
            }
            
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
                if (!arrays_.contains(t.name)) { std::ostringstream m; m << "TypeError: array '" << t.name << "' not DIM'd @ " << sw->pos.line << ':' << sw->pos.col; throw SemanticError(m.str()); }
                const auto& dims = arrays_.at(t.name);
                if (t.indices.size() != dims.size()) { std::ostringstream m; m << "ArityError: array '" << t.name << "' expects " << dims.size() << " indices @ " << sw->pos.line << ':' << sw->pos.col; throw SemanticError(m.str()); }
                for (const auto& idx : t.indices) {
                    if (typeOf(idx.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: array index must be numeric @ " << sw->pos.line << ':' << sw->pos.col; throw SemanticError(m.str()); }
                    analyzeExpr(idx.get());
                }
                isString = varNameIsString(t.name);
            }
        };
        bool lstr = false;
        bool rstr = false;
        checkRef(sw->left, lstr);
        checkRef(sw->right, rstr);
        if (lstr != rstr) { std::ostringstream m; m << "TypeError: SWAP operands must be both numeric or both string @ " << sw->pos.line << ':' << sw->pos.col; throw SemanticError(m.str()); }
        return;
    }
    if (auto bl = dyn_cast<const BloadStmt>(s)) {
        
        if (!dyn_cast<StringExpr>(bl->filename.get())) { std::ostringstream m; m << "TypeError: BLOAD filename must be string @ " << bl->pos.line << ':' << bl->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(bl->filename.get());
        if (bl->offset) {
            if (typeOf(bl->offset.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: BLOAD offset must be numeric @ " << bl->pos.line << ':' << bl->pos.col; throw SemanticError(m.str()); }
            analyzeExpr(bl->offset.get());
        }
        return;
    }
    if (auto bs = dyn_cast<const BsaveStmt>(s)) {
        
        if (!dyn_cast<StringExpr>(bs->filename.get())) { std::ostringstream m; m << "TypeError: BSAVE filename must be string @ " << bs->pos.line << ':' << bs->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(bs->filename.get());
        if (typeOf(bs->offset.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: BSAVE offset must be numeric @ " << bs->pos.line << ':' << bs->pos.col; throw SemanticError(m.str()); }
        if (typeOf(bs->length.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: BSAVE length must be numeric @ " << bs->pos.line << ':' << bs->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(bs->offset.get());
        analyzeExpr(bs->length.get());
        return;
    }
    if (auto pk = dyn_cast<const PokeStmt>(s)) {
        
        if (typeOf(pk->address.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: POKE address must be numeric @ " << pk->pos.line << ':' << pk->pos.col; throw SemanticError(m.str()); }
        if (typeOf(pk->value.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: POKE value must be numeric @ " << pk->pos.line << ':' << pk->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(pk->address.get());
        analyzeExpr(pk->value.get());
        return;
    }
    if (auto ca = dyn_cast<const CallAbsStmt>(s)) {
        
        if (typeOf(ca->address.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CALL address must be numeric @ " << ca->pos.line << ':' << ca->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(ca->address.get());
        return;
    }
    if (auto du = dyn_cast<const DefUsrStmt>(s)) {
        
        if (typeOf(du->address.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: DEF USR address must be numeric @ " << du->pos.line << ':' << du->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(du->address.get());
        return;
    }
    if (auto cd = dyn_cast<const ChdirStmt>(s)) {
        
        if (typeOf(cd->path.get()) != ValueType::String) {
            std::ostringstream m; m << "TypeError: CHDIR requires string path @ " << cd->pos.line << ':' << cd->pos.col; throw SemanticError(m.str());
        }
        analyzeExpr(cd->path.get());
        return;
    }
    if (auto cs = dyn_cast<const ColorStmt>(s)) {
        if (cs->fg && typeOf(cs->fg.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: COLOR fg must be numeric @ " << cs->pos.line << ':' << cs->pos.col; throw SemanticError(m.str()); }
        if (cs->bg && typeOf(cs->bg.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: COLOR bg must be numeric @ " << cs->pos.line << ':' << cs->pos.col; throw SemanticError(m.str()); }
        if (cs->border && typeOf(cs->border.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: COLOR border must be numeric @ " << cs->pos.line << ':' << cs->pos.col; throw SemanticError(m.str()); }
        if (cs->fg) analyzeExpr(cs->fg.get());
        if (cs->bg) analyzeExpr(cs->bg.get());
        if (cs->border) analyzeExpr(cs->border.get());
        return;
    }
    if (auto sc = dyn_cast<const ScreenStmt>(s)) {
        // All provided arguments must be numeric expressions if present
        if (sc->mode && typeOf(sc->mode.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: SCREEN mode must be numeric @ " << sc->pos.line << ':' << sc->pos.col; throw SemanticError(m.str()); }
        if (sc->colorSwitch && typeOf(sc->colorSwitch.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: SCREEN colorswitch must be numeric @ " << sc->pos.line << ':' << sc->pos.col; throw SemanticError(m.str()); }
        if (sc->aPage && typeOf(sc->aPage.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: SCREEN apage must be numeric @ " << sc->pos.line << ':' << sc->pos.col; throw SemanticError(m.str()); }
        if (sc->vPage && typeOf(sc->vPage.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: SCREEN vpage must be numeric @ " << sc->pos.line << ':' << sc->pos.col; throw SemanticError(m.str()); }
        if (sc->mode) analyzeExpr(sc->mode.get());
        if (sc->colorSwitch) analyzeExpr(sc->colorSwitch.get());
        if (sc->aPage) analyzeExpr(sc->aPage.get());
        if (sc->vPage) analyzeExpr(sc->vPage.get());
        
        return;
    }
    if (auto cc = dyn_cast<const CircleStmt>(s)) {
        if (typeOf(cc->x.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE x must be numeric @ " << cc->pos.line << ':' << cc->pos.col; throw SemanticError(m.str()); }
        if (typeOf(cc->y.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE y must be numeric @ " << cc->pos.line << ':' << cc->pos.col; throw SemanticError(m.str()); }
        if (typeOf(cc->r.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE r must be numeric @ " << cc->pos.line << ':' << cc->pos.col; throw SemanticError(m.str()); }
        if (cc->color && typeOf(cc->color.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE color must be numeric @ " << cc->pos.line << ':' << cc->pos.col; throw SemanticError(m.str()); }
        if (cc->start && typeOf(cc->start.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE start must be numeric @ " << cc->pos.line << ':' << cc->pos.col; throw SemanticError(m.str()); }
        if (cc->end && typeOf(cc->end.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE end must be numeric @ " << cc->pos.line << ':' << cc->pos.col; throw SemanticError(m.str()); }
        if (cc->aspect && typeOf(cc->aspect.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: CIRCLE aspect must be numeric @ " << cc->pos.line << ':' << cc->pos.col; throw SemanticError(m.str()); }
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
            std::ostringstream m; m << "SemanticError: duplicate DEF for function '" << df->fnName << "' @ " << df->pos.line << ':' << df->pos.col; throw SemanticError(m.str());
        }
        // Analyze body with param considered local; suppress global reference tracking for it
        currentFnParam_ = df->paramName;
        // Validate return type vs. body type
        const bool retIsString = (!df->fnName.empty() && df->fnName.back() == '$');
        const ValueType bt = typeOf(df->body.get());
        if (retIsString && bt != ValueType::String) { std::ostringstream m; m << "TypeError: DEF " << df->fnName << " must return string; got numeric @ " << df->pos.line << ':' << df->pos.col; throw SemanticError(m.str()); }
        if (!retIsString && bt == ValueType::String) { std::ostringstream m; m << "TypeError: DEF " << df->fnName << " must return number; got string @ " << df->pos.line << ':' << df->pos.col; throw SemanticError(m.str()); }
        analyzeExpr(df->body.get());
        currentFnParam_.reset();
        userFunctions_[up] = df;
        
        return;
    }
    if (dyn_cast<const ClearStmt>(s)) { return; }
    if (auto li = dyn_cast<const LineInputStmt>(s)) {
        // Ensure target is a string variable
        if (!varNameIsString(li->name)) {
            std::ostringstream m; m << "TypeError: LINE INPUT requires string variable @ " << li->pos.line << ':' << li->pos.col;
            throw SemanticError(m.str());
        }
        reference(li->name, li->pos);
        return;
    }
    if (auto g = dyn_cast<const GotoStmt>(s)) {
        if (!lines_.contains(g->targetLine)) {
            if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing GOTO target line " << g->targetLine << " @ " << g->pos.line << ':' << g->pos.col; throw SemanticError(err.str()); }
        }
        return;
    }
    if (auto gs = dyn_cast<const GosubStmt>(s)) {
        if (!lines_.contains(gs->targetLine)) {
            if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing GOSUB target line " << gs->targetLine << " @ " << gs->pos.line << ':' << gs->pos.col; throw SemanticError(err.str()); }
        }
        return;
    }
    if (isa<const ReturnStmt>(s)) { return; }
    if (isa<const EndStmt>(s)) { return; }
    if (auto rz = dyn_cast<const RandomizeStmt>(s)) {
        
        if (rz->seed) {
            if (typeOf(rz->seed.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: RANDOMIZE requires numeric seed @ " << rz->pos.line << ':' << rz->pos.col; throw SemanticError(m.str()); }
            analyzeExpr(rz->seed.get());
        }
        return;
    }
    if (auto og = dyn_cast<const OnGotoStmt>(s)) {
        if (typeOf(og->index.get()) == ValueType::String) {
            std::ostringstream m; m << "TypeError: ON index cannot be string @ " << og->pos.line << ':' << og->pos.col; throw SemanticError(m.str());
        }
        analyzeExpr(og->index.get());
        for (const int ln : og->targets) {
            if (!lines_.contains(ln)) { if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing ON GOTO target line " << ln << " @ " << og->pos.line << ':' << og->pos.col; throw SemanticError(err.str()); } }
        }
        return;
    }
    if (auto ogs = dyn_cast<const OnGosubStmt>(s)) {
        if (typeOf(ogs->index.get()) == ValueType::String) {
            std::ostringstream m; m << "TypeError: ON index cannot be string @ " << ogs->pos.line << ':' << ogs->pos.col; throw SemanticError(m.str());
        }
        analyzeExpr(ogs->index.get());
        for (const int ln : ogs->targets) {
            if (!lines_.contains(ln)) { if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing ON GOSUB target line " << ln << " @ " << ogs->pos.line << ':' << ogs->pos.col; throw SemanticError(err.str()); } }
        }
        return;
    }
    if (auto w = dyn_cast<const WhileStmt>(s)) {
        if (typeOf(w->cond.get()) == ValueType::String) { std::ostringstream m; m << "TypeError: WHILE condition cannot be string @ " << w->pos.line << ':' << w->pos.col; throw SemanticError(m.str()); }
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
            
        }
        return;
    }
    if (auto ch = dyn_cast<const ChainStmt>(s)) {
        if (ch->targetLine.has_value() && !lines_.contains(*ch->targetLine)) {
            if (strictControlFlow_) { std::ostringstream err; err << "ControlFlowError: missing CHAIN target line " << *ch->targetLine << " @ " << ch->pos.line << ':' << ch->pos.col; throw SemanticError(err.str()); }
        }
        
        return;
    }
}


// NOLINTEND(readability-function-cognitive-complexity,readability-function-size,readability-identifier-length,readability-braces-around-statements,readability-qualified-auto,llvm-qualified-auto,readability-implicit-bool-conversion,readability-simplify-boolean-expr,llvm-prefer-isa-or-dyn-cast-in-conditionals,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

} // namespace gwbasic
