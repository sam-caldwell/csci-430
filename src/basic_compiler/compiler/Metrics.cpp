// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/LineInputStmt.h"
#include "basic_compiler/ast/CallExpr.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdio>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unistd.h>

namespace gwbasic {

namespace {

// Depth and count traversal helpers
static std::size_t exprDepth(const Expr* e) {
    if (!e) return 0;
    if (dyn_cast<const NumberExpr>(e)) return 1;
    if (dyn_cast<const StringExpr>(e)) return 1;
    if (dyn_cast<const CallExpr>(e)) {
        const auto* c = static_cast<const CallExpr*>(e);
        std::size_t md = 0;
        for (const auto& a : c->args) md = std::max(md, exprDepth(a.get()));
        return 1 + md;
    }
    if (dyn_cast<const UnaryExpr>(e)) {
        const auto* u = static_cast<const UnaryExpr*>(e);
        return 1 + exprDepth(u->inner.get());
    }
    if (dyn_cast<const BinaryExpr>(e)) {
        const auto* b = static_cast<const BinaryExpr*>(e);
        const auto dl = exprDepth(b->lhs.get());
        const auto dr = exprDepth(b->rhs.get());
        return 1 + (dl > dr ? dl : dr);
    }
    return 1;
}

static void countExprs(const Expr* e, std::size_t& n, std::size_t& depthSum, std::size_t& maxDepth) {
    if (!e) return;
    ++n;
    const auto d = exprDepth(e);
    depthSum += d;
    if (d > maxDepth) maxDepth = d;
    if (const auto* u = dyn_cast<const UnaryExpr>(e)) {
        countExprs(u->inner.get(), n, depthSum, maxDepth);
    } else if (const auto* b = dyn_cast<const BinaryExpr>(e)) {
        countExprs(b->lhs.get(), n, depthSum, maxDepth);
        countExprs(b->rhs.get(), n, depthSum, maxDepth);
    } else if (const auto* c = dyn_cast<const CallExpr>(e)) {
        for (const auto& a : c->args) countExprs(a.get(), n, depthSum, maxDepth);
    }
}

static void countStmtExprs(const Stmt* s, std::size_t& n, std::size_t& depthSum, std::size_t& maxDepth) {
    if (!s) return;
    if (const auto* as = dyn_cast<const AssignStmt>(s)) {
        countExprs(as->value.get(), n, depthSum, maxDepth);
    } else if (const auto* aa = dyn_cast<const ArrayAssignStmt>(s)) {
        countExprs(aa->value.get(), n, depthSum, maxDepth);
        for (const auto& idx : aa->indices) countExprs(idx.get(), n, depthSum, maxDepth);
    } else if (const auto* ma = dyn_cast<const MidAssignStmt>(s)) {
        // MID$(...)= assigns from a string value with start and optional len
        countExprs(ma->value.get(), n, depthSum, maxDepth);
        countExprs(ma->start.get(), n, depthSum, maxDepth);
        if (ma->len) countExprs(ma->len.get(), n, depthSum, maxDepth);
        for (const auto& idx : ma->indices) countExprs(idx.get(), n, depthSum, maxDepth);
    } else if (const auto* ps = dyn_cast<const PrintStmt>(s)) {
        if (ps->value) countExprs(ps->value.get(), n, depthSum, maxDepth);
        for (const auto& v : ps->more) countExprs(v.get(), n, depthSum, maxDepth);
    } else if (const auto* is = dyn_cast<const IfStmt>(s)) {
        countExprs(is->cond.get(), n, depthSum, maxDepth);
    } else if (const auto* ib = dyn_cast<const IfBlockStmt>(s)) {
        countExprs(ib->cond.get(), n, depthSum, maxDepth);
        for (const auto& st : ib->thenBody) countStmtExprs(st.get(), n, depthSum, maxDepth);
        for (const auto& st : ib->elseBody) countStmtExprs(st.get(), n, depthSum, maxDepth);
    } else if (const auto* fs = dyn_cast<const ForStmt>(s)) {
        countExprs(fs->start.get(), n, depthSum, maxDepth);
        countExprs(fs->end.get(), n, depthSum, maxDepth);
        if (fs->step) countExprs(fs->step.get(), n, depthSum, maxDepth);
        for (const auto& st : fs->body) countStmtExprs(st.get(), n, depthSum, maxDepth);
    } else if (const auto* ws = dyn_cast<const WhileStmt>(s)) {
        countExprs(ws->cond.get(), n, depthSum, maxDepth);
        for (const auto& st : ws->body) countStmtExprs(st.get(), n, depthSum, maxDepth);
    } else if (const auto* inp = dyn_cast<const InputStmt>(s)) {
        // InputStmt has no prompt expression; nothing to count
        (void)inp;
    } else if (const auto* lin = dyn_cast<const LineInputStmt>(s)) {
        // LineInputStmt has no prompt expression; nothing to count
        (void)lin;
    } else if (const auto* wr = dyn_cast<const WriteStmt>(s)) {
        for (const auto& e : wr->items) countExprs(e.get(), n, depthSum, maxDepth);
    }
}

} // namespace

void Metrics::computeAstSnapshot(const Program& prog, AstSnapshot& out) {
    out = {}; // reset
    out.lines = prog.lines.size();
    std::size_t depth_sum = 0;
    for (const auto& line : prog.lines) {
        out.statements += line.statements.size();
        for (const auto& st : line.statements) {
            countStmtExprs(st.get(), out.expressions, depth_sum, out.max_expr_depth);
        }
    }
    if (out.expressions > 0) out.avg_expr_depth = static_cast<double>(depth_sum) / static_cast<double>(out.expressions);
}

// Count IR instructions heuristically by scanning lines that look like instructions.
std::size_t Metrics::countIrInstructions(std::string_view ir_text) {
    std::size_t count = 0;
    std::size_t pos = 0;
    while (pos < ir_text.size()) {
        std::size_t end = ir_text.find('\n', pos);
        if (end == std::string_view::npos) end = ir_text.size();
        std::string_view line = ir_text.substr(pos, end - pos);
        // Trim left spaces
        std::size_t i = 0;
        while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) ++i;
        if (i >= 2) { // indented -> likely an instruction or directive
            std::string_view t = line.substr(i);
            // Exclude comments, attribute blocks, metadata and empty
            if (!t.empty() && t[0] != ';' && t.rfind("attributes", 0) != 0 &&
                t.rfind("declare ", 0) != 0 && t.rfind("target ", 0) != 0 &&
                t.rfind("source_filename ", 0) != 0 && t.rfind("!", 0) != 0) {
                // Labels are not indented in our emission, so treat any indented line as instruction
                ++count;
            }
        }
        pos = (end < ir_text.size()) ? end + 1 : ir_text.size();
    }
    return count;
}

static std::string writeTempFile(const std::string& prefix, const std::string& content, const char* ext) {
    namespace fs = std::filesystem;
    fs::path dir = fs::temp_directory_path();
    for (int i = 0; i < 1000; ++i) {
        fs::path p = dir / (prefix + std::to_string(::getpid()) + "_" + std::to_string(i) + ext);
        if (!fs::exists(p)) {
            std::ofstream f(p, std::ios::binary);
            f << content;
            f.close();
            return p.string();
        }
    }
    return {};
}

std::vector<std::pair<std::string, std::size_t>>
Metrics::optimizedIrInstructionCounts(const std::string& ir_text,
                                      const std::string& clang_path,
                                      const std::vector<std::string>& phases) {
    std::vector<std::pair<std::string, std::size_t>> out;
    if (clang_path.empty()) return out;
    // Write IR to a temp file
    const std::string inLl = writeTempFile("gwb_ir_", ir_text, ".ll");
    if (inLl.empty()) return out;
    for (const auto& ph : phases) {
        // Produce optimized IR for this phase using clang
        // Example: clang -S -emit-llvm -x ir -O2 in.ll -o out.ll
        const std::string outLl = writeTempFile("gwb_ir_opt_", std::string{}, ".ll");
        if (outLl.empty()) break;
        std::ostringstream cmd;
        cmd << '"' << clang_path << '"' << " -S -emit-llvm -x ir " << ph
            << ' ' << '"' << inLl << '"' << " -o " << '"' << outLl << '"';
        int ec = std::system(cmd.str().c_str());
        std::size_t cnt = 0;
        if (ec == 0) {
            std::ifstream f(outLl);
            std::stringstream buf;
            buf << f.rdbuf();
            cnt = countIrInstructions(buf.str());
        }
        out.emplace_back(ph, cnt);
        // Best effort cleanup
        std::error_code ig;
        std::filesystem::remove(outLl, ig);
    }
    std::error_code ig;
    std::filesystem::remove(inLl, ig);
    return out;
}

static void printRow(std::ostream& os, std::string_view left, std::string_view right, std::size_t w1, std::size_t w2) {
    os << "| " << left;
    if (left.size() < w1) os << std::string(w1 - left.size(), ' ');
    os << " | " << right;
    if (right.size() < w2) os << std::string(w2 - right.size(), ' ');
    os << " |\n";
}

static void printSep(std::ostream& os, std::size_t w1, std::size_t w2) {
    os << "+-" << std::string(w1, '-') << "-+-" << std::string(w2, '-') << "-+\n";
}

void Metrics::print(std::ostream& os) const {
    // Collect rows
    std::vector<std::pair<std::string, std::string>> rows;
    auto add = [&](std::string k, std::string v) { rows.emplace_back(std::move(k), std::move(v)); };

    add("Phase", "Lexer");
    add("  tokens", std::to_string(lexer_.token_count));

    add("Phase", "AST (parsed)");
    add("  lines", std::to_string(ast_parsed_.lines));
    add("  statements", std::to_string(ast_parsed_.statements));
    add("  expressions", std::to_string(ast_parsed_.expressions));
    add("  max expr depth", std::to_string(ast_parsed_.max_expr_depth));
    add("  avg expr depth", std::to_string(ast_parsed_.avg_expr_depth));

    add("Phase", "AST (after semantics)");
    add("  lines", std::to_string(ast_after_semantics_.lines));
    add("  statements", std::to_string(ast_after_semantics_.statements));
    add("  expressions", std::to_string(ast_after_semantics_.expressions));
    add("  max expr depth", std::to_string(ast_after_semantics_.max_expr_depth));
    add("  avg expr depth", std::to_string(ast_after_semantics_.avg_expr_depth));

    add("Phase", "AST (optimized)");
    add("  lines", std::to_string(ast_after_opt_.lines));
    add("  statements", std::to_string(ast_after_opt_.statements));
    add("  expressions", std::to_string(ast_after_opt_.expressions));
    add("  max expr depth", std::to_string(ast_after_opt_.max_expr_depth));
    add("  avg expr depth", std::to_string(ast_after_opt_.avg_expr_depth));

    add("Phase", "Semantics Opt");
    add("  const-folds", std::to_string(semantics_opt_.const_folds));
    add("  fold add/sub/mul/div", std::to_string(semantics_opt_.fold_add) + "/" +
                                  std::to_string(semantics_opt_.fold_sub) + "/" +
                                  std::to_string(semantics_opt_.fold_mul) + "/" +
                                  std::to_string(semantics_opt_.fold_div));
    add("  fold cmp", std::to_string(semantics_opt_.fold_cmp));
    add("  unary + eliminated", std::to_string(semantics_opt_.unary_elim_plus));
    add("  unary - const-fold", std::to_string(semantics_opt_.unary_const_minus));
    add("  id: +0/-0/*1/*0/ /1", std::to_string(semantics_opt_.id_add_zero) + "/" +
                                 std::to_string(semantics_opt_.id_sub_zero) + "/" +
                                 std::to_string(semantics_opt_.id_mul_one) + "/" +
                                 std::to_string(semantics_opt_.id_mul_zero) + "/" +
                                 std::to_string(semantics_opt_.id_div_one));
    add("  IF const true/false", std::to_string(semantics_opt_.if_const_true_to_goto) + "/" +
                                 std::to_string(semantics_opt_.if_const_false_removed));
    add("  FOR step elided", std::to_string(semantics_opt_.for_step_elided));
    add("  algebraic simplifications", std::to_string(semantics_opt_.algebraic_simplifications));

    add("Phase", "Codegen");
    add("  IR instructions", std::to_string(codegen_.ir_instructions));

    if (!codegen_.opt_phase_ir_counts.empty()) {
        add("Phase", "Codegen Opt");
        for (const auto& [ph, cnt] : codegen_.opt_phase_ir_counts) {
            add(std::string("  ") + ph, std::to_string(cnt));
        }
    }

    // Determine column widths
    std::size_t w1 = 0, w2 = 0;
    for (const auto& [k, v] : rows) { w1 = std::max(w1, k.size()); w2 = std::max(w2, v.size()); }
    printSep(os, w1, w2);
    printRow(os, "Metric", "Value", w1, w2);
    printSep(os, w1, w2);
    for (const auto& [k, v] : rows) printRow(os, k, v, w1, w2);
    printSep(os, w1, w2);
}

void Metrics::print() const { print(std::cout); }

} // namespace gwbasic
