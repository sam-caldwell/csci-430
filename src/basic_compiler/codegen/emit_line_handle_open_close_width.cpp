// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/CloseStmt.h"
#include "basic_compiler/ast/OpenStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/WidthStmt.h"
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
    if (const auto *openStmt = dyn_cast<OpenStmt>(stmt)) {
        const std::string fnptr = emitExpr(out, openStmt->filename.get(), "");
        const std::string mode = nextTemp();
        if (openStmt->mode == FileMode::Input) {
            out << std::format("  {} = getelementptr inbounds i8, ptr @.mode_r, i64 0", mode) << Symbols::LF;
        } else {
            out << std::format("  {} = getelementptr inbounds i8, ptr @.mode_w, i64 0", mode) << Symbols::LF;
        }
        const std::string filePtr = nextTemp();
        out << std::format("  {} = call ptr @fopen(ptr {}, ptr {})", filePtr, fnptr, mode) << Symbols::LF;
        const int idx = openStmt->channel - 1;
        const std::string elemPtr = nextTemp();
        out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", elemPtr, idx) << Symbols::LF;
        out << std::format("  store ptr {}, ptr {}", filePtr, elemPtr) << Symbols::LF;
        return;
    }
    if (const auto *closeStmt = dyn_cast<CloseStmt>(stmt)) {
        const int idx = closeStmt->channel - 1;
        const std::string elemPtr = nextTemp();
        out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", elemPtr, idx) << Symbols::LF;
        const std::string fileHandle = nextTemp();
        out << std::format("  {} = load ptr, ptr {}", fileHandle, elemPtr) << Symbols::LF;
        out << std::format("  call i32 @fclose(ptr {})", fileHandle) << Symbols::LF;
        out << std::format("  store ptr null, ptr {}", elemPtr) << Symbols::LF;
        return;
    }
    if (const auto *widthStmt = dyn_cast<WidthStmt>(stmt)) {
        std::string targetPtr;
        if (widthStmt->device) {
            const std::string dev = emitExpr(out, widthStmt->device.get(), "");
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
        const std::string dval = emitExpr(out, widthStmt->columns.get(), "");
        const std::string widthI32 = nextTemp(); out << std::format("  {} = fptosi double {} to i32", widthI32, dval) << Symbols::LF;
        const std::string lt1 = nextTemp(); out << std::format("  {} = icmp slt i32 {}, 1", lt1, widthI32) << Symbols::LF;
        const std::string widthMin = nextTemp(); out << std::format("  {} = select i1 {}, i32 1, i32 {}", widthMin, lt1, widthI32) << Symbols::LF;
        const std::string isScrSel = nextTemp(); out << std::format("  {} = icmp eq ptr {}, @gwb_screen_cols", isScrSel, targetPtr) << Symbols::LF;
        const std::string maxScr = nextTemp(); out << std::format("  {} = select i1 {}, i32 80, i32 255", maxScr, isScrSel) << Symbols::LF;
        const std::string gtMax = nextTemp(); out << std::format("  {} = icmp sgt i32 {}, {}", gtMax, widthMin, maxScr) << Symbols::LF;
        const std::string widthClamped = nextTemp(); out << std::format("  {} = select i1 {}, i32 {}, i32 {}", widthClamped, gtMax, maxScr, widthMin) << Symbols::LF;
        out << std::format("  store i32 {}, ptr {}", widthClamped, targetPtr) << Symbols::LF;
        const std::string doClamp = nextTemp(); out << std::format("  {} = icmp eq ptr {}, @gwb_screen_cols", doClamp, targetPtr) << Symbols::LF;
        const std::string afterLbl = std::format("{}_wd_after_{}", currLineLabel, ++localCounter);
        const std::string clampLbl = std::format("{}_wd_c_{}", currLineLabel, localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", doClamp, clampLbl, afterLbl) << Symbols::LF;
        out << clampLbl << ":" << Symbols::LF;
        const std::string cm1 = nextTemp(); out << std::format("  {} = sub i32 {}, 1", cm1, widthClamped) << Symbols::LF;
        const std::string col = nextTemp(); out << std::format("  {} = load i32, ptr @gwb_cur_col", col) << Symbols::LF;
        const std::string gtCol = nextTemp(); out << std::format("  {} = icmp sgt i32 {}, {}", gtCol, col, cm1) << Symbols::LF;
        const std::string colClamped = nextTemp(); out << std::format("  {} = select i1 {}, i32 {}, i32 {}", colClamped, gtCol, cm1, col) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_cur_col", colClamped) << Symbols::LF;
        out << std::format("  br label %{}", afterLbl) << Symbols::LF;
        out << afterLbl << ":" << Symbols::LF;
        (void)currLineLabel; // used for label naming
    }
}

} // namespace gwbasic
