// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include <sstream>
#include <format>
#include <cmath>

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
void CodeGenerator::emitForHandlePrint(std::ostringstream& out, const PrintStmt* pr, const std::string& currLineLabel, int& localCounter) {
    std::vector<const Expr*> items;
    if (pr->value) items.push_back(pr->value.get());
    for (const auto& v : pr->more) items.push_back(v.get());
    auto emit_pad_to_next_zone = [&]() { emitForPrintPadZone(out, pr); };
    if (items.empty()) {
        if (pr->channel >= 1) {
            auto fptr = nextTemp();
            auto fh = nextTemp();
            out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1) << Symbols::LF
                << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF
                << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr @.fmt_str, ptr @.empty)", fh) << Symbols::LF;
        } else {
            out << "  call i32 (ptr, ...) @printf(ptr @.fmt_str, ptr @.empty)" << Symbols::LF;
            auto sbuf = nextTemp();
            auto n = nextTemp();
            auto n64 = nextTemp();
            out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF
                << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr @.fmt_str, ptr @.empty)", n, sbuf) << Symbols::LF
                << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF
                << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64)
                << Symbols::LF;
        }
        return;
    }
    for (size_t pi = 0; pi < items.size(); ++pi) {
        const bool last = (pi + 1 == items.size());
        const bool addNL = last && (pr->trail == PrintStmt::Terminator::Newline);
        if (const auto * se = dyn_cast<const StringExpr>(items[pi])) { emitForPrintStringItem(out, pr, se, addNL); continue; }
        // Constant number fast path
        if (const auto cnum = dyn_cast<const NumberExpr>(items[pi])) {
            bool nextStartsWithSpace = false;
            if (!last && (pi + 1) < items.size()) {
                if (const auto* ns = dyn_cast<const StringExpr>(items[pi + 1])) {
                    if (!ns->value.empty() && ns->value.front() == ' ') nextStartsWithSpace = true;
                }
            }
            emitForPrintConstNumberItem(out, pr, cnum->value, addNL, nextStartsWithSpace);
            continue;
        }
        // General numeric case
        auto val = emitExpr(out, items[pi], currLineLabel);
        bool hasOverride = (pr->format != nullptr);
        bool nextStartsWithSpace = false;
        if (!last && (pi + 1) < items.size()) {
            if (const auto* ns = dyn_cast<const StringExpr>(items[pi + 1])) {
                if (!ns->value.empty() && ns->value.front() == ' ') nextStartsWithSpace = true;
            }
        }
        if (hasOverride) {
            emitForPrintDynamicOverride(out, pr, val, currLineLabel, localCounter);
            if (!last && pi < pr->seps.size() && pr->seps[pi] == PrintStmt::Sep::Comma) emit_pad_to_next_zone();
            continue;
        }
        emitForPrintDynamicAuto(out, pr, val, addNL, nextStartsWithSpace, currLineLabel, localCounter);
        if (!last && pi < pr->seps.size() && pr->seps[pi] == PrintStmt::Sep::Comma) emit_pad_to_next_zone();
    }
    if (pr->trail == PrintStmt::Terminator::Newline) {
        if (pr->channel >= 1) {
            auto fptr = nextTemp();
            auto fh = nextTemp();
            out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", fptr, pr->channel - 1) << Symbols::LF
                << std::format("  {} = load ptr, ptr {}", fh, fptr) << Symbols::LF
                << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr @.fmt_str, ptr @.nl)", fh) << Symbols::LF;
        } else {
            out << "  call i32 (ptr, ...) @printf(ptr @.fmt_str, ptr @.nl)" << Symbols::LF;
            auto sbuf = nextTemp();
            auto n = nextTemp();
            auto n64 = nextTemp();
            out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF
                << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr @.fmt_str, ptr @.nl)", n, sbuf) << Symbols::LF
                << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF
                << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64)
                << Symbols::LF;
        }
    } else if (pr->trail == PrintStmt::Terminator::Comma) {
        emit_pad_to_next_zone();
    }
}

} // namespace gwbasic
