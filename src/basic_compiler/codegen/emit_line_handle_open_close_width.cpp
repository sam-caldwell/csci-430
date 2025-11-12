// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/OpenStmt.h"
#include "basic_compiler/ast/CloseStmt.h"
#include "basic_compiler/ast/WidthStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

// OPEN / CLOSE / WIDTH
// NOLINTNEXTLINE(readability-function-size,readability-identifier-length)
void CodeGenerator::emitLineHandleOpenCloseWidth(std::ostringstream &out,
                                                 const Stmt *stmt,
                                                 const std::string &currLineLabel,
                                                 int &localCounter) {
    if (const auto *op = dyn_cast<OpenStmt>(stmt)) {
        const std::string fnptr = emitExpr(out, op->filename.get(), "");
        const std::string mode = nextTemp();
        if (op->mode == FileMode::Input) {
            out << std::format("  {} = getelementptr inbounds i8, ptr @.mode_r, i64 0", mode) << Symbols::LF;
        } else {
            out << std::format("  {} = getelementptr inbounds i8, ptr @.mode_w, i64 0", mode) << Symbols::LF;
        }
        const std::string f = nextTemp();
        out << std::format("  {} = call ptr @fopen(ptr {}, ptr {})", f, fnptr, mode) << Symbols::LF;
        const int idx = op->channel - 1;
        const std::string ep = nextTemp();
        out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", ep, idx) << Symbols::LF;
        out << std::format("  store ptr {}, ptr {}", f, ep) << Symbols::LF;
        return;
    }
    if (const auto *cl = dyn_cast<CloseStmt>(stmt)) {
        const int idx = cl->channel - 1;
        const std::string ep = nextTemp();
        out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", ep, idx) << Symbols::LF;
        const std::string fh = nextTemp();
        out << std::format("  {} = load ptr, ptr {}", fh, ep) << Symbols::LF;
        out << std::format("  call i32 @fclose(ptr {})", fh) << Symbols::LF;
        out << std::format("  store ptr null, ptr {}", ep) << Symbols::LF;
        return;
    }
    if (const auto *wd = dyn_cast<WidthStmt>(stmt)) {
        std::string targetPtr;
        if (wd->device) {
            const std::string dev = emitExpr(out, wd->device.get(), "");
            const std::string scr = nextTemp(); out << std::format("  {} = getelementptr inbounds [6 x i8], ptr @.str_scrn, i64 0, i64 0", scr) << Symbols::LF;
            const std::string lpt = nextTemp(); out << std::format("  {} = getelementptr inbounds [6 x i8], ptr @.str_lpt1, i64 0, i64 0", lpt) << Symbols::LF;
            const std::string cmpScr = nextTemp(); out << std::format("  {} = call i32 @strcmp(ptr {}, ptr {})", cmpScr, dev, scr) << Symbols::LF;
            const std::string isScr = nextTemp(); out << std::format("  {} = icmp eq i32 {}, 0", isScr, cmpScr) << Symbols::LF;
            const std::string cmpLpt = nextTemp(); out << std::format("  {} = call i32 @strcmp(ptr {}, ptr {})", cmpLpt, dev, lpt) << Symbols::LF;
            const std::string isLpt = nextTemp(); out << std::format("  {} = icmp eq i32 {}, 0", isLpt, cmpLpt) << Symbols::LF;
            const std::string scrLbl = std::format("{}_wd_scr_{}", currLineLabel, ++localCounter);
            const std::string lptLbl = std::format("{}_wd_lpt_{}", currLineLabel, localCounter);
            const std::string mergeLbl = std::format("{}_wd_merge_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", isScr, scrLbl, lptLbl) << Symbols::LF;
            out << scrLbl << ":" << Symbols::LF;
            out << std::format("  br label %{}", mergeLbl) << Symbols::LF;
            out << lptLbl << ":" << Symbols::LF;
            out << std::format("  br label %{}", mergeLbl) << Symbols::LF;
            out << mergeLbl << ":" << Symbols::LF;
            const std::string scrPtr = nextTemp(); out << std::format("  {} = inttoptr i64 ptrtoint (ptr @gwb_screen_cols to i64) to ptr", scrPtr) << Symbols::LF;
            const std::string lptPtr = nextTemp(); out << std::format("  {} = inttoptr i64 ptrtoint (ptr @gwb_printer_cols to i64) to ptr", lptPtr) << Symbols::LF;
            const std::string tgt = nextTemp(); out << std::format("  {} = select i1 {}, ptr {}, ptr {}", tgt, isScr, scrPtr, lptPtr) << Symbols::LF;
            targetPtr = tgt;
        } else {
            targetPtr = "@gwb_screen_cols";
        }
        const std::string dval = emitExpr(out, wd->columns.get(), "");
        const std::string wi = nextTemp(); out << std::format("  {} = fptosi double {} to i32", wi, dval) << Symbols::LF;
        const std::string lt1 = nextTemp(); out << std::format("  {} = icmp slt i32 {}, 1", lt1, wi) << Symbols::LF;
        const std::string w1 = nextTemp(); out << std::format("  {} = select i1 {}, i32 1, i32 {}", w1, lt1, wi) << Symbols::LF;
        const std::string isScrSel = nextTemp(); out << std::format("  {} = icmp eq ptr {}, @gwb_screen_cols", isScrSel, targetPtr) << Symbols::LF;
        const std::string maxScr = nextTemp(); out << std::format("  {} = select i1 {}, i32 80, i32 255", maxScr, isScrSel) << Symbols::LF;
        const std::string gtMax = nextTemp(); out << std::format("  {} = icmp sgt i32 {}, {}", gtMax, w1, maxScr) << Symbols::LF;
        const std::string wc = nextTemp(); out << std::format("  {} = select i1 {}, i32 {}, i32 {}", wc, gtMax, maxScr, w1) << Symbols::LF;
        out << std::format("  store i32 {}, ptr {}", wc, targetPtr) << Symbols::LF;
        const std::string doClamp = nextTemp(); out << std::format("  {} = icmp eq ptr {}, @gwb_screen_cols", doClamp, targetPtr) << Symbols::LF;
        const std::string afterLbl = std::format("{}_wd_after_{}", currLineLabel, ++localCounter);
        const std::string clampLbl = std::format("{}_wd_c_{}", currLineLabel, localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", doClamp, clampLbl, afterLbl) << Symbols::LF;
        out << clampLbl << ":" << Symbols::LF;
        const std::string cm1 = nextTemp(); out << std::format("  {} = sub i32 {}, 1", cm1, wc) << Symbols::LF;
        const std::string col = nextTemp(); out << std::format("  {} = load i32, ptr @gwb_cur_col", col) << Symbols::LF;
        const std::string gtCol = nextTemp(); out << std::format("  {} = icmp sgt i32 {}, {}", gtCol, col, cm1) << Symbols::LF;
        const std::string cc = nextTemp(); out << std::format("  {} = select i1 {}, i32 {}, i32 {}", cc, gtCol, cm1, col) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_cur_col", cc) << Symbols::LF;
        out << std::format("  br label %{}", afterLbl) << Symbols::LF;
        out << afterLbl << ":" << Symbols::LF;
        log() << "line " << currentLine_ << " WidthStmt -> set width" << Symbols::LF;
        (void)currLineLabel; // used for label naming
    }
}

} // namespace gwbasic
