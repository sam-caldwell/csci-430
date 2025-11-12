// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/StringExpr.h"
#include <sstream>
#include <string>
#include <vector>
#include <format>
#include "basic_compiler/ast/RTTI.h"

namespace gwbasic {

// NOLINTBEGIN(readability-function-cognitive-complexity,readability-function-size,readability-identifier-length,readability-avoid-nested-conditional-operator,readability-braces-around-statements)
void CodeGenerator::emitLineHandlePrint(std::ostringstream &out,
                                        const PrintStmt *pr,
                                        const std::string &currLineLabel,
                                        int &localCounter) {
    std::vector<const Expr *> items;
    if (pr->value) { items.push_back(pr->value.get()); }
    for (const auto &v: pr->more) { items.push_back(v.get()); }

    auto emit_pad_to_next_zone = [&] {
        if (!printZones_) { return; }
        const std::string col = nextTemp();
        out << std::format("  {} = load i32, ptr @gwb_cur_col", col) << Symbols::LF;
        const std::string mod = nextTemp();
        out << std::format("  {} = srem i32 {}, 14", mod, col) << Symbols::LF;
        const std::string isZero = nextTemp();
        out << std::format("  {} = icmp eq i32 {}, 0", isZero, mod) << Symbols::LF;
        const std::string sub = nextTemp();
        out << std::format("  {} = sub i32 14, {}", sub, mod) << Symbols::LF;
        const std::string pad = nextTemp();
        out << std::format("  {} = select i1 {}, i32 14, i32 {}", pad, isZero, sub) << Symbols::LF;
        const std::string fmt = nextTemp();
        out << std::format("  {} = getelementptr inbounds i8, ptr @.fmt_pad, i64 0", fmt) << Symbols::LF;
        const std::string spaces = nextTemp();
        out << std::format("  {} = getelementptr inbounds [15 x i8], ptr @.spaces_14, i64 0, i64 0", spaces) << Symbols::LF;
        if (pr->channel >= 1) {
            const std::string filePtr = nextTemp();
            out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", filePtr, pr->channel - 1) << Symbols::LF;
            const std::string fileHandle = nextTemp();
            out << std::format("  {} = load ptr, ptr {}", fileHandle, filePtr) << Symbols::LF;
            out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, i32 {}, ptr {})", fileHandle, fmt, pad, spaces) << Symbols::LF;
        } else {
            out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 {}, ptr {})", fmt, pad, spaces) << Symbols::LF;
            const std::string sbuf = nextTemp();
            out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
            const std::string writtenChars = nextTemp();
            out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i32 {}, ptr {})", writtenChars, sbuf, fmt, pad, spaces) << Symbols::LF;
            const std::string writtenChars64 = nextTemp();
            out << std::format("  {} = sext i32 {} to i64", writtenChars64, writtenChars) << Symbols::LF;
            out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, writtenChars64) << Symbols::LF;
        }
    };

    for (size_t pi = 0; pi < items.size(); ++pi) {
        const bool last = (pi + 1 == items.size());
        const bool addNL = last && (pr->trail == PrintStmt::Terminator::Newline);
        const Expr *itemExpr = items[pi];
        if (isStringExpr(itemExpr)) {
            const std::string sptr = emitExpr(out, itemExpr, currLineLabel);
            const std::string defFmt = nextTemp();
            {
                const char *sym = addNL ? "@.fmt_str" : "@.fmt_str_sp";
                out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 0", defFmt, sym) << Symbols::LF;
            }
            std::string useFmt = defFmt;
            if (pr->format) { useFmt = emitExpr(out, pr->format.get(), currLineLabel); }
            if (pr->channel >= 1) {
                const std::string filePtr = nextTemp();
                out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", filePtr, pr->channel - 1) << Symbols::LF;
                const std::string fileHandle = nextTemp();
                out << std::format("  {} = load ptr, ptr {}", fileHandle, filePtr) << Symbols::LF;
                out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr {}, ptr {})", fileHandle, useFmt, sptr) << Symbols::LF;
            } else {
                out << std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", useFmt, sptr) << Symbols::LF;
                const std::string sbuf = nextTemp();
                out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
                const std::string writtenChars = nextTemp();
                out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, ptr {})", writtenChars, sbuf, useFmt, sptr) << Symbols::LF;
                const std::string writtenChars64 = nextTemp();
                out << std::format("  {} = sext i32 {} to i64", writtenChars64, writtenChars) << Symbols::LF;
                out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, writtenChars64) << Symbols::LF;
            }
        } else {
            bool nextStartsWithSpace = false;
            if (!last && (pi + 1) < items.size()) {
                if (const auto *ns = dyn_cast<const StringExpr>(items[pi + 1])) {
                    if (!ns->value.empty() && ns->value.front() == ' ') { nextStartsWithSpace = true; }
                }
            }
            const std::string val = emitExpr(out, itemExpr, currLineLabel);
            if (pr->format) {
                emitForPrintDynamicOverride(out, pr, val, currLineLabel, localCounter);
            } else {
                emitForPrintDynamicAuto(out, pr, val, addNL, nextStartsWithSpace, currLineLabel, localCounter);
            }
        }
        if (!last) {
            const std::size_t idx = pi;
            if (idx < pr->seps.size() && pr->seps[idx] == PrintStmt::Sep::Comma) {
                emit_pad_to_next_zone();
            }
        }
    }
    if (pr->trail == PrintStmt::Terminator::Newline) {
        if (pr->channel >= 1) {
            const std::string filePtr = nextTemp();
            out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", filePtr, pr->channel - 1) << Symbols::LF;
            const std::string fileHandle = nextTemp();
            out << std::format("  {} = load ptr, ptr {}", fileHandle, filePtr) << Symbols::LF;
            const std::string fmtPtr = nextTemp();
            out << std::format("  {} = getelementptr inbounds [3 x i8], ptr @.nl, i64 0, i64 0", fmtPtr) << Symbols::LF;
            out << std::format("  call i32 (ptr, ...) @fprintf(ptr {}, ptr @.fmt_str, ptr {})", fileHandle, fmtPtr) << Symbols::LF;
        } else {
            out << "  call i32 (ptr, ...) @printf(ptr @.fmt_str, ptr @.nl)" << Symbols::LF;
            const std::string sbuf = nextTemp();
            const std::string writtenChars = nextTemp();
            const std::string writtenChars64 = nextTemp();
            out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
            out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr @.fmt_str, ptr @.nl)", writtenChars, sbuf) << Symbols::LF;
            out << std::format("  {} = sext i32 {} to i64", writtenChars64, writtenChars) << Symbols::LF;
            out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, writtenChars64) << Symbols::LF;
        }
    }
}
// NOLINTEND(readability-function-cognitive-complexity,readability-function-size,readability-identifier-length,readability-avoid-nested-conditional-operator,readability-braces-around-statements)

} // namespace gwbasic
