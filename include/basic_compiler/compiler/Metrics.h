// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace gwbasic {

struct Program; // fwd decl

/**
 * Class: Metrics
 * Purpose:
 *  - Self-contained metrics collector across compiler phases.
 *  - Provides increment/set APIs and prints a summary table.
 */
class Metrics {
public:
    // Construction: default all counters to zero
    Metrics() = default;

    // Control analysis-only mode (prevents optimizer mutations during counting)
    void setAnalyzeOnly(bool v) { analyze_only_ = v; }
    [[nodiscard]] bool isAnalyzeOnly() const { return analyze_only_; }

    // Lexer
    void incToken() { ++lexer_.token_count; }

    // AST snapshots
    void recordParsedSnapshot(const Program& prog) { computeAstSnapshot(prog, ast_parsed_); }
    void recordAfterSemanticsSnapshot(const Program& prog) { computeAstSnapshot(prog, ast_after_semantics_); }
    void recordOptimizedSnapshot(const Program& prog) { computeAstSnapshot(prog, ast_after_opt_); }

    // Semantics/optimizer increment APIs
    void incConstFoldAdd() { ++semantics_opt_.const_folds; ++semantics_opt_.fold_add; }
    void incConstFoldSub() { ++semantics_opt_.const_folds; ++semantics_opt_.fold_sub; }
    void incConstFoldMul() { ++semantics_opt_.const_folds; ++semantics_opt_.fold_mul; }
    void incConstFoldDiv() { ++semantics_opt_.const_folds; ++semantics_opt_.fold_div; }
    void incConstFoldCmp() { ++semantics_opt_.const_folds; ++semantics_opt_.fold_cmp; }

    void incUnaryElimPlus() { ++semantics_opt_.unary_elim_plus; ++semantics_opt_.algebraic_simplifications; }
    void incUnaryConstMinus() { ++semantics_opt_.unary_const_minus; ++semantics_opt_.const_folds; }

    void incIdAddZero() { ++semantics_opt_.id_add_zero; ++semantics_opt_.algebraic_simplifications; }
    void incIdSubZero() { ++semantics_opt_.id_sub_zero; ++semantics_opt_.algebraic_simplifications; }
    void incIdMulOne() { ++semantics_opt_.id_mul_one; ++semantics_opt_.algebraic_simplifications; }
    void incIdMulZero() { ++semantics_opt_.id_mul_zero; ++semantics_opt_.algebraic_simplifications; }
    void incIdDivOne() { ++semantics_opt_.id_div_one; ++semantics_opt_.algebraic_simplifications; }

    void incIfConstTrueToGoto() { ++semantics_opt_.if_const_true_to_goto; }
    void incIfConstFalseRemoved() { ++semantics_opt_.if_const_false_removed; }
    void incForStepElided() { ++semantics_opt_.for_step_elided; }

    // Codegen
    void setIrInstructionCount(std::size_t n) { codegen_.ir_instructions = n; }
    void setOptPhaseCounts(std::vector<std::pair<std::string, std::size_t>> v) {
        codegen_.opt_phase_ir_counts = std::move(v);
    }

    // Counting helpers
    static std::size_t countIrInstructions(std::string_view ir_text);
    static std::vector<std::pair<std::string, std::size_t>>
    optimizedIrInstructionCounts(const std::string& ir_text,
                                 const std::string& clang_path,
                                 const std::vector<std::string>& phases);

    // Print summary table to stdout or provided stream
    void print() const; // stdout
    void print(std::ostream& os) const; // custom stream

private:
    // Data containers
    struct LexerMetrics { std::size_t token_count{0}; } lexer_{};
    struct AstSnapshot { std::size_t lines{0}, statements{0}, expressions{0}, max_expr_depth{0}; double avg_expr_depth{0.0}; };
    AstSnapshot ast_parsed_{}, ast_after_semantics_{}, ast_after_opt_{};
    struct SemanticsOptMetrics {
        std::size_t const_folds{0};
        std::size_t fold_add{0}, fold_sub{0}, fold_mul{0}, fold_div{0}, fold_cmp{0};
        std::size_t unary_elim_plus{0}, unary_const_minus{0};
        std::size_t id_add_zero{0}, id_sub_zero{0}, id_mul_one{0}, id_mul_zero{0}, id_div_one{0};
        std::size_t if_const_true_to_goto{0}, if_const_false_removed{0};
        std::size_t for_step_elided{0};
        std::size_t algebraic_simplifications{0};
    } semantics_opt_{};
    struct CodegenMetrics {
        std::size_t ir_instructions{0};
        std::vector<std::pair<std::string, std::size_t>> opt_phase_ir_counts;
    } codegen_{};

    bool analyze_only_{false};

    // Internal utilities
    static void computeAstSnapshot(const Program& prog, AstSnapshot& out);

    // Friend accessors for tests if needed
    friend class MetricsAccessorForTests;
};

// Global metrics context (optional)
inline Metrics* gMetrics = nullptr;

} // namespace gwbasic
