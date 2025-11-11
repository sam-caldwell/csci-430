// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include <cstddef>
#include <format>
#include <sstream>
#include <string>
#include <vector>

static std::vector<const gwbasic::Expr*> collectItems(const gwbasic::PrintStmt* print_stmt) {
    std::vector<const gwbasic::Expr*> items;
    if (print_stmt->value) { items.push_back(print_stmt->value.get()); }
    for (const auto& value_expr : print_stmt->more) { items.push_back(value_expr.get()); }
    return items;
}

static bool nextStartsWithSpaceHelper(const std::vector<const gwbasic::Expr*>& items, size_t index) {
    if (index + 1 >= items.size()) { return false; }
    if (const auto* next_str = gwbasic::dyn_cast<const gwbasic::StringExpr>(items[index + 1])) {
        return !next_str->value.empty() && next_str->value.front() == ' ';
    }
    return false;
}

static bool shouldPadZone(const gwbasic::PrintStmt* print_stmt, bool last, size_t pos) {
    return !last && pos < print_stmt->seps.size() && print_stmt->seps[pos] == gwbasic::PrintStmt::Sep::Comma;
}

// Note: next two helpers are implemented as local lambdas in emitForHandlePrint


namespace gwbasic {

/*
 * Function: emitForHandlePrint
 * Summary: Emit IR for a PRINT statement inside a FOR body.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - pr: Parsed PrintStmt node.
 *  - currLineLabel: Base label for naming emitted blocks.
 *  - localCounter: Per-line counter to uniquify labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitForHandlePrint(std::ostringstream& out, const PrintStmt* print_stmt, const std::string& currLineLabel, int& localCounter) {
    auto items = collectItems(print_stmt);

    auto emit_empty = [&] {
        if (print_stmt->channel >= 1) {
            auto fptr = nextTemp();
            auto file_handle = nextTemp();
            out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, print_stmt->channel - 1) << Symbols::LF
                << std::format("  {} = load ptr, ptr {}", file_handle, fptr) << Symbols::LF
                << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr @.fmt_str, ptr @.empty)", file_handle) << Symbols::LF;
        } else {
            out << "  call i32 (ptr, ...) @printf(ptr @.fmt_str, ptr @.empty)" << Symbols::LF;
            auto sbuf = nextTemp();
            auto n_chars = nextTemp();
            auto n64 = nextTemp();
            out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF
                << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr @.fmt_str, ptr @.empty)", n_chars, sbuf) << Symbols::LF
                << std::format("  {} = sext i32 {} to i64", n64, n_chars) << Symbols::LF
                << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
        }
    };
    auto flush_nl = [&] {
        if (print_stmt->channel >= 1) {
            auto fptr = nextTemp();
            auto file_handle = nextTemp();
            out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, print_stmt->channel - 1) << Symbols::LF
                << std::format("  {} = load ptr, ptr {}", file_handle, fptr) << Symbols::LF
                << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr @.fmt_str, ptr @.nl)", file_handle) << Symbols::LF;
        } else {
            out << "  call i32 (ptr, ...) @printf(ptr @.fmt_str, ptr @.nl)" << Symbols::LF;
            auto sbuf = nextTemp();
            auto n_chars = nextTemp();
            auto n64 = nextTemp();
            out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF
                << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr @.fmt_str, ptr @.nl)", n_chars, sbuf) << Symbols::LF
                << std::format("  {} = sext i32 {} to i64", n64, n_chars) << Symbols::LF
                << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
        }
    };
    auto handle_const_num = [&](double value, bool add_nl, bool next_space) {
        emitForPrintConstNumberItem(out, print_stmt, value, add_nl, next_space);
    };
    auto handle_dynamic_num = [&](const std::string& value, bool has_override, bool add_nl, bool next_space, bool last, size_t idx) {
        if (has_override) {
            emitForPrintDynamicOverride(out, print_stmt, value, currLineLabel, localCounter);
            if (shouldPadZone(print_stmt, last, idx)) { emitForPrintPadZone(out, print_stmt); }
            return;
        }
        emitForPrintDynamicAuto(out, print_stmt, value, add_nl, next_space, currLineLabel, localCounter);
        if (shouldPadZone(print_stmt, last, idx)) { emitForPrintPadZone(out, print_stmt); }
    };

    if (items.empty()) { emit_empty(); return; }
    for (size_t pi = 0; pi < items.size(); ++pi) {
        const bool last = (pi + 1 == items.size());
        const bool addNL = last && (print_stmt->trail == PrintStmt::Terminator::Newline);
        if (const auto* const str_expr = dyn_cast<const StringExpr>(items[pi])) {
            emitForPrintStringItem(out, print_stmt, str_expr, addNL);
            continue;
        }
        // Constant number fast path
        if (const auto* const cnum = dyn_cast<const NumberExpr>(items[pi])) {
            const bool starts_with_space = nextStartsWithSpaceHelper(items, pi);
            handle_const_num(cnum->value, addNL, starts_with_space);
            continue;
        }
        // General numeric case
        auto val = emitExpr(out, items[pi], currLineLabel);
        const bool hasOverride = (print_stmt->format != nullptr);
        const bool starts_with_space = nextStartsWithSpaceHelper(items, pi);
        handle_dynamic_num(val, hasOverride, addNL, starts_with_space, last, pi);
    }
    if (print_stmt->trail == PrintStmt::Terminator::Newline) {
        flush_nl();
    } else if (print_stmt->trail == PrintStmt::Terminator::Comma) {
        emitForPrintPadZone(out, print_stmt);
    }
}

} // namespace gwbasic
