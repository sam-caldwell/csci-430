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
#include "basic_compiler/ast/EraseStmt.h"
#include "basic_compiler/ast/SwapStmt.h"

namespace gwbasic {
    void CodeGenerator::collectVarsForBeforeLineFromStmt(const Stmt *s, std::set<std::string> &vars,
                                                         std::set<std::string> &arrays) {
        if (!s) return;
        if (const auto a = dyn_cast<const AssignStmt>(s)) {
            vars.insert(a->name);
            collectVarsForBeforeLineFromExpr(a->value.get(), vars, arrays);
            return;
        }
        if (const auto aa = dyn_cast<const ArrayAssignStmt>(s)) {
            arrays.insert(aa->name);
            for (const auto &ix: aa->indices)
                collectVarsForBeforeLineFromExpr(ix.get(), vars, arrays);
            collectVarsForBeforeLineFromExpr(aa->value.get(), vars, arrays);
            return;
        }
        if (const auto ib = dyn_cast<const IfBlockStmt>(s)) {
            collectVarsForBeforeLineFromExpr(ib->cond.get(), vars, arrays);
            for (const auto &st: ib->thenBody)
                collectVarsForBeforeLineFromStmt(st.get(), vars, arrays);
            for (const auto &st: ib->elseBody)
                collectVarsForBeforeLineFromStmt(st.get(), vars, arrays);
            return;
        }
        if (const auto is = dyn_cast<const IfStmt>(s)) {
            collectVarsForBeforeLineFromExpr(is->cond.get(), vars, arrays);
            return;
        }
        if (const auto fs = dyn_cast<const ForStmt>(s)) {
            vars.insert(fs->var);
            collectVarsForBeforeLineFromExpr(fs->start.get(), vars, arrays);
            collectVarsForBeforeLineFromExpr(fs->end.get(), vars, arrays);
            if (fs->step)
                collectVarsForBeforeLineFromExpr(fs->step.get(), vars, arrays);
            for (const auto &st: fs->body)
                collectVarsForBeforeLineFromStmt(st.get(), vars, arrays);
            return;
        }
        if (const auto ws = dyn_cast<const WhileStmt>(s)) {
            collectVarsForBeforeLineFromExpr(ws->cond.get(), vars, arrays);
            for (const auto &st: ws->body)
                collectVarsForBeforeLineFromStmt(st.get(), vars, arrays);
            return;
        }
        if (const auto pr = dyn_cast<const PrintStmt>(s)) {
            if (pr->value)
                collectVarsForBeforeLineFromExpr(pr->value.get(), vars, arrays);
            for (const auto &v: pr->more)
                collectVarsForBeforeLineFromExpr(v.get(), vars, arrays);
            return;
        }
        if (const auto in = dyn_cast<const InputStmt>(s)) {
            for (const auto &v: in->variables)
                vars.insert(v);
            return;
        }
        if (const auto rd = dyn_cast<const ReadStmt>(s)) {
            for (const auto &[name, indices]: rd->targets) {
                if (!indices.empty()) {
                    arrays.insert(name);
                    for (const auto &ix: indices)
                        collectVarsForBeforeLineFromExpr(ix.get(), vars, arrays);
                } else {
                    vars.insert(name);
                }
            }
            return;
        }
        if (const auto ds = dyn_cast<const DimStmt>(s)) {
            arrays.insert(ds->name);
            return;
        }
        if (const auto sw = dyn_cast<const SwapStmt>(s)) {
            auto handle = [&](const ReadTarget &t) {
                if (!t.indices.empty()) {
                    arrays.insert(t.name);
                    for (const auto &ix: t.indices)
                        collectVarsForBeforeLineFromExpr(ix.get(), vars, arrays);
                } else {
                    vars.insert(t.name);
                }
            };
            handle(sw->left);
            handle(sw->right);
            return;
        }
        if (const auto er = dyn_cast<const EraseStmt>(s)) {
            for (const auto &n: er->names)
                arrays.erase(n);
            return;
        }
        if (const auto wr = dyn_cast<const WriteStmt>(s)) {
            for (const auto &e: wr->items)
                collectVarsForBeforeLineFromExpr(e.get(), vars, arrays);
            return;
        }
        if (const auto og = dyn_cast<const OnGotoStmt>(s)) {
            collectVarsForBeforeLineFromExpr(og->index.get(), vars, arrays);
            return;
        }
        if (const auto ogs = dyn_cast<const OnGosubStmt>(s)) {
            collectVarsForBeforeLineFromExpr(ogs->index.get(), vars, arrays);
        }
    }
} // namespace gwbasic
