// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/DimStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"

namespace gwbasic {

void CodeGenerator::collectVarsForBeforeLineFromStmt(const Stmt* s, std::set<std::string>& vars, std::set<std::string>& arrays) {
    if (!s) return;
    if (auto a = dyn_cast<const AssignStmt>(s)) { vars.insert(a->name); collectVarsForBeforeLineFromExpr(a->value.get(), vars, arrays); return; }
    if (auto aa = dyn_cast<const ArrayAssignStmt>(s)) { arrays.insert(aa->name); collectVarsForBeforeLineFromExpr(aa->index.get(), vars, arrays); collectVarsForBeforeLineFromExpr(aa->value.get(), vars, arrays); return; }
    if (auto ib = dyn_cast<const IfBlockStmt>(s)) { collectVarsForBeforeLineFromExpr(ib->cond.get(), vars, arrays); for (const auto& st : ib->thenBody) collectVarsForBeforeLineFromStmt(st.get(), vars, arrays); for (const auto& st : ib->elseBody) collectVarsForBeforeLineFromStmt(st.get(), vars, arrays); return; }
    if (auto is = dyn_cast<const IfStmt>(s)) { collectVarsForBeforeLineFromExpr(is->cond.get(), vars, arrays); return; }
    if (auto fs = dyn_cast<const ForStmt>(s)) { vars.insert(fs->var); collectVarsForBeforeLineFromExpr(fs->start.get(), vars, arrays); collectVarsForBeforeLineFromExpr(fs->end.get(), vars, arrays); if (fs->step) collectVarsForBeforeLineFromExpr(fs->step.get(), vars, arrays); for (const auto& st : fs->body) collectVarsForBeforeLineFromStmt(st.get(), vars, arrays); return; }
    if (auto ws = dyn_cast<const WhileStmt>(s)) { collectVarsForBeforeLineFromExpr(ws->cond.get(), vars, arrays); for (const auto& st : ws->body) collectVarsForBeforeLineFromStmt(st.get(), vars, arrays); return; }
    if (auto pr = dyn_cast<const PrintStmt>(s)) { if (pr->value) collectVarsForBeforeLineFromExpr(pr->value.get(), vars, arrays); for (const auto& v : pr->more) collectVarsForBeforeLineFromExpr(v.get(), vars, arrays); return; }
    if (auto in = dyn_cast<const InputStmt>(s)) { vars.insert(in->name); return; }
    if (auto rd = dyn_cast<const ReadStmt>(s)) {
        for (const auto& t : rd->targets) {
            if (t.index) { arrays.insert(t.name); collectVarsForBeforeLineFromExpr(t.index.get(), vars, arrays); }
            else { vars.insert(t.name); }
        }
        return;
    }
    if (auto ds = dyn_cast<const DimStmt>(s)) { arrays.insert(ds->name); return; }
    if (auto wr = dyn_cast<const WriteStmt>(s)) { for (const auto& e : wr->items) collectVarsForBeforeLineFromExpr(e.get(), vars, arrays); return; }
    if (auto og = dyn_cast<const OnGotoStmt>(s)) { collectVarsForBeforeLineFromExpr(og->index.get(), vars, arrays); return; }
    if (auto ogs = dyn_cast<const OnGosubStmt>(s)) { collectVarsForBeforeLineFromExpr(ogs->index.get(), vars, arrays); return; }
}

} // namespace gwbasic
