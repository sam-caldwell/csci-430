// (c) 2025 Sam Caldwell. All Rights Reserved.
// NOLINTBEGIN(llvm-include-order,misc-include-cleaner)
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

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>
#include <unistd.h>
#include <utility>
#include <vector>
// NOLINTEND(llvm-include-order,misc-include-cleaner)

namespace gwbasic {

// Depth and count traversal helpers
static std::size_t exprDepth(const Expr* expr) {
    if (expr == nullptr) { return 0; }
    if (dyn_cast<const NumberExpr>(expr) != nullptr) { return 1; }
    if (dyn_cast<const StringExpr>(expr) != nullptr) { return 1; }
    if (const auto* call = dyn_cast<const CallExpr>(expr)) {
        std::size_t maxDepthLocal = 0;
        for (const auto& arg : call->args) { maxDepthLocal = std::max(maxDepthLocal, exprDepth(arg.get())); }
        return 1 + maxDepthLocal;
    }
    if (const auto* unary = dyn_cast<const UnaryExpr>(expr)) {
        return 1 + exprDepth(unary->inner.get());
    }
    if (const auto* bin = dyn_cast<const BinaryExpr>(expr)) {
        const auto leftDepth = exprDepth(bin->lhs.get());
        const auto rightDepth = exprDepth(bin->rhs.get());
        return 1 + std::max(leftDepth, rightDepth);
    }
    return 1;
}

static void countExprs(const Expr* expr, std::size_t& n, std::size_t& depthSum, std::size_t& maxDepth) {
    if (expr == nullptr) { return; }
    ++n;
    const auto depthVal = exprDepth(expr);
    depthSum += depthVal;
    maxDepth = std::max(depthVal, maxDepth);
    if (const auto* unary = dyn_cast<const UnaryExpr>(expr)) {
        countExprs(unary->inner.get(), n, depthSum, maxDepth);
    } else if (const auto* bin = dyn_cast<const BinaryExpr>(expr)) {
        countExprs(bin->lhs.get(), n, depthSum, maxDepth);
        countExprs(bin->rhs.get(), n, depthSum, maxDepth);
    } else if (const auto* call = dyn_cast<const CallExpr>(expr)) {
        for (const auto& arg : call->args) { countExprs(arg.get(), n, depthSum, maxDepth); }
    }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity,readability-identifier-length,misc-include-cleaner)
static void countStmtExprs(const Stmt* stmt, std::size_t& n, std::size_t& depthSum, std::size_t& maxDepth) {
    if (stmt == nullptr) { return; }
    if (const auto* assignStmt = dyn_cast<const AssignStmt>(stmt)) {
        countExprs(assignStmt->value.get(), n, depthSum, maxDepth);
    } else if (const auto* arrayAssign = dyn_cast<const ArrayAssignStmt>(stmt)) {
        countExprs(arrayAssign->value.get(), n, depthSum, maxDepth);
        for (const auto& indexExpr : arrayAssign->indices) { countExprs(indexExpr.get(), n, depthSum, maxDepth); }
    } else if (const auto* midAssign = dyn_cast<const MidAssignStmt>(stmt)) {
        // MID$(...)= assigns from a string value with start and optional len
        countExprs(midAssign->value.get(), n, depthSum, maxDepth);
        countExprs(midAssign->start.get(), n, depthSum, maxDepth);
        if (midAssign->len) { countExprs(midAssign->len.get(), n, depthSum, maxDepth); }
        for (const auto& indexExpr : midAssign->indices) { countExprs(indexExpr.get(), n, depthSum, maxDepth); }
    } else if (const auto* printStmt = dyn_cast<const PrintStmt>(stmt)) {
        if (printStmt->value) { countExprs(printStmt->value.get(), n, depthSum, maxDepth); }
        for (const auto& valueExpr : printStmt->more) { countExprs(valueExpr.get(), n, depthSum, maxDepth); }
    } else if (const auto* ifStmt = dyn_cast<const IfStmt>(stmt)) {
        countExprs(ifStmt->cond.get(), n, depthSum, maxDepth);
    } else if (const auto* ifBlock = dyn_cast<const IfBlockStmt>(stmt)) {
        countExprs(ifBlock->cond.get(), n, depthSum, maxDepth);
        for (const auto& nestedStmt : ifBlock->thenBody) { countStmtExprs(nestedStmt.get(), n, depthSum, maxDepth); }
        for (const auto& nestedStmt : ifBlock->elseBody) { countStmtExprs(nestedStmt.get(), n, depthSum, maxDepth); }
    } else if (const auto* forStmt = dyn_cast<const ForStmt>(stmt)) {
        countExprs(forStmt->start.get(), n, depthSum, maxDepth);
        countExprs(forStmt->end.get(), n, depthSum, maxDepth);
        if (forStmt->step) { countExprs(forStmt->step.get(), n, depthSum, maxDepth); }
        for (const auto& nestedStmt : forStmt->body) { countStmtExprs(nestedStmt.get(), n, depthSum, maxDepth); }
    } else if (const auto* whileStmt = dyn_cast<const WhileStmt>(stmt)) {
        countExprs(whileStmt->cond.get(), n, depthSum, maxDepth);
        for (const auto& nestedStmt : whileStmt->body) { countStmtExprs(nestedStmt.get(), n, depthSum, maxDepth); }
    } else if (const auto* inp = dyn_cast<const InputStmt>(stmt)) {
        // InputStmt has no prompt expression; nothing to count
        (void)inp;
    } else if (const auto* lin = dyn_cast<const LineInputStmt>(stmt)) {
        // LineInputStmt has no prompt expression; nothing to count
        (void)lin;
    } else if (const auto* writeStmt = dyn_cast<const WriteStmt>(stmt)) {
        for (const auto& exprItem : writeStmt->items) { countExprs(exprItem.get(), n, depthSum, maxDepth); }
    }
}

// end helpers

void Metrics::computeAstSnapshot(const Program& prog, AstSnapshot& out) {
    out = {}; // reset
    out.lines = prog.lines.size();
    std::size_t depthSum = 0;
    for (const auto& line : prog.lines) {
        out.statements += line.statements.size();
        for (const auto& stmtUptr : line.statements) {
            countStmtExprs(stmtUptr.get(), out.expressions, depthSum, out.max_expr_depth);
        }
    }
    if (out.expressions > 0) { out.avg_expr_depth = static_cast<double>(depthSum) / static_cast<double>(out.expressions); }
}

// Count IR instructions heuristically by scanning lines that look like instructions.
// NOLINTNEXTLINE(readability-function-size,misc-include-cleaner)
std::size_t Metrics::countIrInstructions(std::string_view ir_text) {
    std::size_t count = 0;
    std::size_t pos = 0;
    while (pos < ir_text.size()) {
        std::size_t end = ir_text.find('\n', pos);
        if (end == std::string_view::npos) { end = ir_text.size(); }
        const std::string_view line = ir_text.substr(pos, end - pos);
        // Trim left spaces
        std::size_t indent = 0;
        while (indent < line.size() && (line[indent] == ' ' || line[indent] == '\t')) { ++indent; }
        if (indent >= 2) { // indented -> likely an instruction or directive
            const std::string_view tail = line.substr(indent);
            // Exclude comments, attribute blocks, metadata and empty
            if (!tail.empty() && tail[0] != ';' && tail.rfind("attributes", 0) != 0 &&
                tail.rfind("declare ", 0) != 0 && tail.rfind("target ", 0) != 0 &&
                tail.rfind("source_filename ", 0) != 0 && tail.rfind("!", 0) != 0) {
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
    const fs::path tempDir = fs::temp_directory_path();
    constexpr int kMaxProbe = 1000; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    for (int i = 0; i < kMaxProbe; ++i) {
        const fs::path tempPath = tempDir / (prefix + std::to_string(::getpid()) + "_" + std::to_string(i) + ext);
        if (!fs::exists(tempPath)) {
            std::ofstream outFile(tempPath, std::ios::binary);
            outFile << content;
            outFile.close();
            return tempPath.string();
        }
    }
    return {};
}

std::vector<std::pair<std::string, std::size_t>>
Metrics::optimizedIrInstructionCounts(const std::string& ir_text,
                                      const std::string& clang_path,
                                      const std::vector<std::string>& phases) {
    std::vector<std::pair<std::string, std::size_t>> out;
    if (clang_path.empty()) { return out; }
    // Write IR to a temp file
    const std::string inLl = writeTempFile("gwb_ir_", ir_text, ".ll");
    if (inLl.empty()) { return out; }
    for (const auto& phaseFlag : phases) {
        // Produce optimized IR for this phase using clang
        // Example: clang -S -emit-llvm -x ir -O2 in.ll -o out.ll
        const std::string outLl = writeTempFile("gwb_ir_opt_", std::string{}, ".ll");
        if (outLl.empty()) { break; }
        std::ostringstream cmd;
        cmd << '"' << clang_path << '"' << " -S -emit-llvm -x ir " << phaseFlag
            << ' ' << '"' << inLl << '"' << " -o " << '"' << outLl << '"';
        const int exitCode = std::system(cmd.str().c_str()); // NOLINT(concurrency-mt-unsafe)
        std::size_t irCount = 0;
        if (exitCode == 0) {
            const std::ifstream inFile(outLl);
            std::stringstream buf;
            buf << inFile.rdbuf();
            irCount = countIrInstructions(buf.str());
        }
        out.emplace_back(phaseFlag, irCount);
        // Best effort cleanup
        std::error_code ignoreEc;
        std::filesystem::remove(outLl, ignoreEc);
    }
    std::error_code ignoreEc;
    std::filesystem::remove(inLl, ignoreEc);
    return out;
}

static void printRow(std::ostream& outStream, std::string_view left, std::string_view right, std::size_t width1, std::size_t width2) {
    outStream << "| " << left;
    if (left.size() < width1) { outStream << std::string(width1 - left.size(), ' '); }
    outStream << " | " << right;
    if (right.size() < width2) { outStream << std::string(width2 - right.size(), ' '); }
    outStream << " |\n";
}

static void printSep(std::ostream& outStream, std::size_t width1, std::size_t width2) {
    outStream << "+-" << std::string(width1, '-') << "-+-" << std::string(width2, '-') << "-+\n";
}

void Metrics::print(std::ostream& out) const {
    // Collect rows
    std::vector<std::pair<std::string, std::string>> rows;
    auto add = [&](std::string key, std::string value) { rows.emplace_back(std::move(key), std::move(value)); };

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
        for (const auto& [phaseFlag, countValue] : codegen_.opt_phase_ir_counts) {
            add(std::string("  ") + phaseFlag, std::to_string(countValue));
        }
    }

    // Determine column widths
    std::size_t col1Width = 0;
    std::size_t col2Width = 0;
    for (const auto& [key, value] : rows) {
        col1Width = std::max(col1Width, key.size());
        col2Width = std::max(col2Width, value.size());
    }
    printSep(out, col1Width, col2Width);
    printRow(out, "Metric", "Value", col1Width, col2Width);
    printSep(out, col1Width, col2Width);
    for (const auto& [key, value] : rows) { printRow(out, key, value, col1Width, col2Width); }
    printSep(out, col1Width, col2Width);
}

void Metrics::print() const { print(std::cout); }

} // namespace gwbasic
