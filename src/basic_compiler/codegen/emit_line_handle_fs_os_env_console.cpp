// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/ClsStmt.h"
#include "basic_compiler/ast/LocateStmt.h"
#include "basic_compiler/ast/ListStmt.h"
#include "basic_compiler/ast/FilesStmt.h"
#include "basic_compiler/ast/MkdirStmt.h"
#include "basic_compiler/ast/RmdirStmt.h"
#include "basic_compiler/ast/KillStmt.h"
#include "basic_compiler/ast/NameStmt.h"
#include "basic_compiler/ast/ShellStmt.h"
#include "basic_compiler/ast/EnvironStmt.h"
#include "basic_compiler/ast/BeepStmt.h"
#include "basic_compiler/ast/ChdirStmt.h"
#include "basic_compiler/ast/ClearStmt.h"
#include "basic_compiler/ast/ColorStmt.h"
#include "basic_compiler/ast/ScreenStmt.h"
#include "basic_compiler/ast/CircleStmt.h"
#include "basic_compiler/ast/DefSegStmt.h"
#include "basic_compiler/ast/PokeStmt.h"
#include "basic_compiler/ast/BloadStmt.h"
#include "basic_compiler/ast/BsaveStmt.h"
#include "basic_compiler/ast/CallAbsStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

// Console/FS/OS/ENV handlers (grouped)
// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity)
void CodeGenerator::emitLineHandleFsOsEnvConsole(std::ostringstream &out,
                                                 const Stmt *stmt,
                                                 const std::string &currLineLabel,
                                                 int &localCounter,
                                                 const std::string &nextLabel) {
    // Memory/segment operations
    if (const auto *ds = dyn_cast<DefSegStmt>(stmt)) {
        if (ds->value) {
            const std::string val = emitExpr(out, ds->value.get(), "");
            const std::string i32v = nextTemp(); out << std::format("  {} = fptosi double {} to i32", i32v, val) << Symbols::LF;
            out << std::format("  store i32 {}, ptr @gwb_seg", i32v) << Symbols::LF;
        } else {
            out << "  store i32 0, ptr @gwb_seg" << Symbols::LF;
        }
        log() << "line " << currentLine_ << " DefSegStmt -> set gwb_seg" << Symbols::LF;
        return;
    }
    if (const auto *pk = dyn_cast<PokeStmt>(stmt)) {
        const std::string seg = nextTemp(); out << "  " << seg << " = load i32, ptr @gwb_seg" << Symbols::LF;
        const std::string seg16 = nextTemp(); out << std::format("  {} = mul i32 {}, 16", seg16, seg) << Symbols::LF;
        const std::string off = emitExpr(out, pk->address.get(), "");
        const std::string off64 = nextTemp(); out << std::format("  {} = fptosi double {} to i64", off64, off) << Symbols::LF;
        const std::string seg64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", seg64, seg16) << Symbols::LF;
        const std::string addr = nextTemp(); out << std::format("  {} = add i64 {}, {}", addr, seg64, off64) << Symbols::LF;
        const std::string base = nextTemp(); out << std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", base, addr) << Symbols::LF;
        const std::string v = emitExpr(out, pk->value.get(), "");
        const std::string vi = nextTemp(); out << std::format("  {} = fptosi double {} to i32", vi, v) << Symbols::LF;
        const std::string vb = nextTemp(); out << std::format("  {} = trunc i32 {} to i8", vb, vi) << Symbols::LF;
        out << std::format("  store i8 {}, ptr {}", vb, base) << Symbols::LF;
        return;
    }
    if (const auto *bl = dyn_cast<BloadStmt>(stmt)) {
        const std::string fnptr = emitExpr(out, bl->filename.get(), "");
        const std::string mode = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr @.mode_rb, i64 0", mode) << Symbols::LF;
        const std::string f = nextTemp(); out << std::format("  {} = call ptr @fopen(ptr {}, ptr {})", f, fnptr, mode) << Symbols::LF;
        const std::string seg = nextTemp(); out << "  " << seg << " = load i32, ptr @gwb_seg" << Symbols::LF;
        const std::string seg16 = nextTemp(); out << std::format("  {} = mul i32 {}, 16", seg16, seg) << Symbols::LF;
        std::string off = "0.0"; if (bl->offset) { off = emitExpr(out, bl->offset.get(), ""); }
        const std::string off64 = nextTemp(); out << std::format("  {} = fptosi double {} to i64", off64, off) << Symbols::LF;
        const std::string seg64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", seg64, seg16) << Symbols::LF;
        const std::string addr = nextTemp(); out << std::format("  {} = add i64 {}, {}", addr, seg64, off64) << Symbols::LF;
        const std::string dest = nextTemp(); out << std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", dest, addr) << Symbols::LF;
        const std::string remain = nextTemp(); out << std::format("  {} = sub i64 1048576, {}", remain, addr) << Symbols::LF;
        out << std::format("  call i64 @fread(ptr {}, i64 1, i64 {}, ptr {})", dest, remain, f) << Symbols::LF;
        out << std::format("  call i32 @fclose(ptr {})", f) << Symbols::LF;
        return;
    }
    if (const auto *bs = dyn_cast<BsaveStmt>(stmt)) {
        const std::string fnptr = emitExpr(out, bs->filename.get(), "");
        const std::string mode = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr @.mode_wb, i64 0", mode) << Symbols::LF;
        const std::string f = nextTemp(); out << std::format("  {} = call ptr @fopen(ptr {}, ptr {})", f, fnptr, mode) << Symbols::LF;
        const std::string seg = nextTemp(); out << "  " << seg << " = load i32, ptr @gwb_seg" << Symbols::LF;
        const std::string seg16 = nextTemp(); out << std::format("  {} = mul i32 {}, 16", seg16, seg) << Symbols::LF;
        const std::string off = emitExpr(out, bs->offset.get(), "");
        const std::string off64 = nextTemp(); out << std::format("  {} = fptosi double {} to i64", off64, off) << Symbols::LF;
        const std::string seg64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", seg64, seg16) << Symbols::LF;
        const std::string addr = nextTemp(); out << std::format("  {} = add i64 {}, {}", addr, seg64, off64) << Symbols::LF;
        const std::string src = nextTemp(); out << std::format("  {} = getelementptr inbounds [1048576 x i8], ptr @gwb_mem, i64 0, i64 {}", src, addr) << Symbols::LF;
        const std::string len = emitExpr(out, bs->length.get(), "");
        const std::string len64 = nextTemp(); out << std::format("  {} = fptosi double {} to i64", len64, len) << Symbols::LF;
        out << std::format("  call i64 @fwrite(ptr {}, i64 1, i64 {}, ptr {})", src, len64, f) << Symbols::LF;
        out << std::format("  call i32 @fclose(ptr {})", f) << Symbols::LF;
        return;
    }
    if (const auto *ca = dyn_cast<CallAbsStmt>(stmt)) {
        const std::string seg = nextTemp(); out << "  " << seg << " = load i32, ptr @gwb_seg" << Symbols::LF;
        const std::string seg16 = nextTemp(); out << std::format("  {} = mul i32 {}, 16", seg16, seg) << Symbols::LF;
        const std::string off = emitExpr(out, ca->address.get(), "");
        const std::string off64 = nextTemp(); out << std::format("  {} = fptosi double {} to i64", off64, off) << Symbols::LF;
        const std::string seg64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", seg64, seg16) << Symbols::LF;
        const std::string addr = nextTemp(); out << std::format("  {} = add i64 {}, {}", addr, seg64, off64) << Symbols::LF;
        out << std::format("  call void @gwb_call(i64 {})", addr) << Symbols::LF;
        log() << "line " << currentLine_ << " CALL invoke" << Symbols::LF;
        return;
    }
    if (isa<ClsStmt>(stmt)) {
        out << std::format("  call ptr @memset(ptr @gwb_screen, i32 0, i64 2000)") << Symbols::LF;
        out << std::format("  store i32 0, ptr @gwb_cur_row") << Symbols::LF;
        out << std::format("  store i32 0, ptr @gwb_cur_col") << Symbols::LF;
        log() << "line " << currentLine_ << " ClsStmt -> clear screen and reset cursor" << Symbols::LF;
        return;
    }
    if (const auto *lc = dyn_cast<LocateStmt>(stmt)) {
        {
            const std::string dr = emitExpr(out, lc->row.get(), "");
            const std::string ri = nextTemp(); out << std::format("  {} = fptosi double {} to i32", ri, dr) << Symbols::LF;
            const std::string lt1 = nextTemp(); out << std::format("  {} = icmp slt i32 {}, 1", lt1, ri) << Symbols::LF;
            const std::string r1 = nextTemp(); out << std::format("  {} = select i1 {}, i32 1, i32 {}", r1, lt1, ri) << Symbols::LF;
            const std::string gt25 = nextTemp(); out << std::format("  {} = icmp sgt i32 {}, 25", gt25, r1) << Symbols::LF;
            const std::string rc = nextTemp(); out << std::format("  {} = select i1 {}, i32 25, i32 {}", rc, gt25, r1) << Symbols::LF;
            const std::string rz = nextTemp(); out << std::format("  {} = sub i32 {}, 1", rz, rc) << Symbols::LF;
            out << std::format("  store i32 {}, ptr @gwb_cur_row", rz) << Symbols::LF;
        }
        if (lc->col) {
            const std::string dc = emitExpr(out, lc->col.get(), "");
            const std::string ci = nextTemp(); out << std::format("  {} = fptosi double {} to i32", ci, dc) << Symbols::LF;
            const std::string lt1c = nextTemp(); out << std::format("  {} = icmp slt i32 {}, 1", lt1c, ci) << Symbols::LF;
            const std::string c1 = nextTemp(); out << std::format("  {} = select i1 {}, i32 1, i32 {}", c1, lt1c, ci) << Symbols::LF;
            const std::string w = nextTemp(); out << std::format("  {} = load i32, ptr @gwb_screen_cols", w) << Symbols::LF;
            const std::string gtW = nextTemp(); out << std::format("  {} = icmp sgt i32 {}, {}", gtW, c1, w) << Symbols::LF;
            const std::string cc = nextTemp(); out << std::format("  {} = select i1 {}, i32 {}, i32 {}", cc, gtW, w, c1) << Symbols::LF;
            const std::string cz = nextTemp(); out << std::format("  {} = sub i32 {}, 1", cz, cc) << Symbols::LF;
            out << std::format("  store i32 {}, ptr @gwb_cur_col", cz) << Symbols::LF;
        }
        log() << "line " << currentLine_ << " LocateStmt -> set row/col" << Symbols::LF;
        return;
    }
    if (const auto *ls = dyn_cast<ListStmt>(stmt)) {
        const int minLine = lineNumbers_.empty() ? 0 : lineNumbers_.front();
        const int maxLine = lineNumbers_.empty() ? 0 : lineNumbers_.back();
        int startBound = minLine;
        int endBound = maxLine;
        if (ls->startLine.has_value()) { startBound = (ls->startIsDot ? currentLine_ : *ls->startLine); }
        if (ls->endLine.has_value()) { endBound = (ls->endIsDot ? currentLine_ : *ls->endLine); }
        for (int ln : lineNumbers_) {
            const std::string c1 = nextTemp(); out << std::format("  {} = icmp sge i32 {}, {}", c1, ln, startBound) << Symbols::LF;
            const std::string c2 = nextTemp(); out << std::format("  {} = icmp sle i32 {}, {}", c2, ln, endBound) << Symbols::LF;
            const std::string inr = nextTemp(); out << std::format("  {} = and i1 {}, {}", inr, c1, c2) << Symbols::LF;
            const std::string doLbl = std::format("{}_lst_do_{}", currLineLabel, ++localCounter);
            const std::string contLbl = std::format("{}_lst_cont_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", inr, doLbl, contLbl) << Symbols::LF;
            out << doLbl << ":" << Symbols::LF;
            const std::string fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds [5 x i8], ptr @.fmt_int, i64 0, i64 0", fmt) << Symbols::LF;
            const std::string ln64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", ln64, ln) << Symbols::LF;
            if (ls->toPrinter) {
                const std::string p0 = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 0", p0) << Symbols::LF;
                const std::string fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, p0) << Symbols::LF;
                const std::string sbuf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
                const std::string n = nextTemp(); out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i64 {})", n, sbuf, fmt, ln64) << Symbols::LF;
                const std::string n64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF;
                const std::string useStd = std::format("{}_lst_lpt_std_{}", currLineLabel, ++localCounter);
                const std::string useLpt = std::format("{}_lst_lpt_dev_{}", currLineLabel, localCounter);
                const std::string isNull = nextTemp(); out << std::format("  {} = icmp eq ptr {}, null", isNull, fh) << Symbols::LF;
                out << std::format("  br i1 {}, label %{}, label %{}", isNull, useStd, useLpt) << Symbols::LF;
                out << useStd << ":" << Symbols::LF;
                out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmt, ln64) << Symbols::LF;
                out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
                out << std::format("  br label %{}", contLbl) << Symbols::LF;
                out << useLpt << ":" << Symbols::LF;
                out << std::format("  call i64 @fwrite(ptr {}, i64 {}, i64 1, ptr {})", sbuf, n64, fh) << Symbols::LF;
            } else {
                out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i64 {})", fmt, ln64) << Symbols::LF;
                const std::string sbuf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf) << Symbols::LF;
                const std::string n = nextTemp(); out << std::format("  {} = call i32 (ptr, i64, ptr, ...) @snprintf(ptr {}, i64 256, ptr {}, i64 {})", n, sbuf, fmt, ln64) << Symbols::LF;
                const std::string n64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", n64, n) << Symbols::LF;
                out << std::format("  call void @gwb_screen_write(ptr {}, i64 {})", sbuf, n64) << Symbols::LF;
            }
            out << std::format("  br label %{}", contLbl) << Symbols::LF;
            out << contLbl << ":" << Symbols::LF;
        }
        log() << "line " << currentLine_ << " ListStmt -> emit listing" << Symbols::LF;
        return;
    }
    if (const auto *fl = dyn_cast<FilesStmt>(stmt)) {
        const std::string dev = (fl->device ? emitExpr(out, fl->device.get(), "") : std::string("null"));
        const std::string pat = (fl->pattern ? emitExpr(out, fl->pattern.get(), "") : std::string("null"));
        out << std::format("  call void @gwb_list_files(ptr {}, ptr {})", dev, pat) << Symbols::LF;
        log() << "line " << currentLine_ << " FilesStmt -> gwb_list_files()" << Symbols::LF;
        return;
    }
    if (const auto *mk = dyn_cast<MkdirStmt>(stmt)) {
        const std::string p = emitExpr(out, mk->path.get(), "");
        const std::string rc = nextTemp(); out << std::format("  {} = call i32 @mkdir(ptr {}, i32 511)", rc, p) << Symbols::LF;
        const std::string ok = nextTemp(); out << std::format("  {} = icmp eq i32 {}, 0", ok, rc) << Symbols::LF;
        const std::string okLbl = std::format("{}_mk_ok_{}", currLineLabel, ++localCounter);
        const std::string errLbl = std::format("{}_mk_err_{}", currLineLabel, localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", ok, okLbl, errLbl) << Symbols::LF;
        out << errLbl << ":" << Symbols::LF;
        emitErrorDispatch(out, 75, currentLine_, 0);
        out << okLbl << ":" << Symbols::LF;
        log() << "line " << currentLine_ << " Mkdir -> mkdir()" << Symbols::LF;
        return;
    }
    if (const auto *rd = dyn_cast<RmdirStmt>(stmt)) {
        const std::string p = emitExpr(out, rd->path.get(), "");
        const std::string rc = nextTemp(); out << std::format("  {} = call i32 @rmdir(ptr {})", rc, p) << Symbols::LF;
        const std::string ok = nextTemp(); out << std::format("  {} = icmp eq i32 {}, 0", ok, rc) << Symbols::LF;
        const std::string okLbl = std::format("{}_rd_ok_{}", currLineLabel, ++localCounter);
        const std::string errLbl = std::format("{}_rd_err_{}", currLineLabel, localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", ok, okLbl, errLbl) << Symbols::LF;
        out << errLbl << ":" << Symbols::LF;
        emitErrorDispatch(out, 75, currentLine_, 0);
        out << okLbl << ":" << Symbols::LF;
        log() << "line " << currentLine_ << " Rmdir -> rmdir()" << Symbols::LF;
        return;
    }
    if (const auto *kl = dyn_cast<KillStmt>(stmt)) {
        const std::string f = emitExpr(out, kl->filespec.get(), "");
        const std::string rc = nextTemp(); out << std::format("  {} = call i32 @remove(ptr {})", rc, f) << Symbols::LF;
        const std::string ok = nextTemp(); out << std::format("  {} = icmp eq i32 {}, 0", ok, rc) << Symbols::LF;
        const std::string okLbl = std::format("{}_kl_ok_{}", currLineLabel, ++localCounter);
        const std::string errLbl = std::format("{}_kl_err_{}", currLineLabel, localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", ok, okLbl, errLbl) << Symbols::LF;
        out << errLbl << ":" << Symbols::LF;
        emitErrorDispatch(out, 75, currentLine_, 0);
        out << okLbl << ":" << Symbols::LF;
        log() << "line " << currentLine_ << " Kill -> remove()" << Symbols::LF;
        return;
    }
    if (const auto *nm = dyn_cast<NameStmt>(stmt)) {
        const std::string oldp = emitExpr(out, nm->oldName.get(), "");
        const std::string newp = emitExpr(out, nm->newName.get(), "");
        const std::string rc = nextTemp(); out << std::format("  {} = call i32 @rename(ptr {}, ptr {})", rc, oldp, newp) << Symbols::LF;
        const std::string ok = nextTemp(); out << std::format("  {} = icmp eq i32 {}, 0", ok, rc) << Symbols::LF;
        const std::string okLbl = std::format("{}_nm_ok_{}", currLineLabel, ++localCounter);
        const std::string errLbl = std::format("{}_nm_err_{}", currLineLabel, localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", ok, okLbl, errLbl) << Symbols::LF;
        out << errLbl << ":" << Symbols::LF;
        emitErrorDispatch(out, 75, currentLine_, 0);
        out << okLbl << ":" << Symbols::LF;
        log() << "line " << currentLine_ << " Name -> rename()" << Symbols::LF;
        return;
    }
    if (const auto *sh = dyn_cast<ShellStmt>(stmt)) {
        if (sh->command) {
            const std::string cmd = emitExpr(out, sh->command.get(), "");
            out << std::format("  call i32 @system(ptr {})", cmd) << Symbols::LF;
        } else {
            log() << "line " << currentLine_ << " Shell(no-arg) (no-op)" << Symbols::LF;
        }
        return;
    }
    if (const auto *ev = dyn_cast<EnvironStmt>(stmt)) {
        const std::string s = emitExpr(out, ev->spec.get(), "");
        const std::string eqp = nextTemp(); out << std::format("  {} = getelementptr inbounds [2 x i8], ptr @.str_eq, i64 0, i64 0", eqp) << Symbols::LF;
        const std::string pos = nextTemp(); out << std::format("  {} = call ptr @strstr(ptr {}, ptr {})", pos, s, eqp) << Symbols::LF;
        const std::string hasEq = nextTemp(); out << std::format("  {} = icmp ne ptr {}, null", hasEq, pos) << Symbols::LF;
        const std::string okLbl = std::format("{}_ev_ok_{}", currLineLabel, ++localCounter);
        const std::string errLbl = std::format("{}_ev_err_{}", currLineLabel, localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", hasEq, okLbl, errLbl) << Symbols::LF;
        out << errLbl << ":" << Symbols::LF;
        emitErrorDispatch(out, 5, currentLine_, 0);
        out << okLbl << ":" << Symbols::LF;
        const std::string s_i = nextTemp(); out << std::format("  {} = ptrtoint ptr {} to i64", s_i, s) << Symbols::LF;
        const std::string p_i = nextTemp(); out << std::format("  {} = ptrtoint ptr {} to i64", p_i, pos) << Symbols::LF;
        const std::string nlen = nextTemp(); out << std::format("  {} = sub i64 {}, {}", nlen, p_i, s_i) << Symbols::LF;
        const std::string nsize = nextTemp(); out << std::format("  {} = add i64 {}, 1", nsize, nlen) << Symbols::LF;
        const std::string nbuf = nextTemp(); out << std::format("  {} = call ptr @malloc(i64 {})", nbuf, nsize) << Symbols::LF;
        out << std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", nbuf, s, nlen) << Symbols::LF;
        const std::string nend = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", nend, nbuf, nlen) << Symbols::LF;
        out << std::format("  store i8 0, ptr {}", nend) << Symbols::LF;
        const std::string valp = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 1", valp, pos) << Symbols::LF;
        const std::string ch = nextTemp(); out << std::format("  {} = load i8, ptr {}", ch, valp) << Symbols::LF;
        const std::string isEmpty = nextTemp(); out << std::format("  {} = icmp eq i8 {}, 0", isEmpty, ch) << Symbols::LF;
        const std::string doUnset = std::format("{}_ev_unset_{}", currLineLabel, ++localCounter);
        const std::string doSet = std::format("{}_ev_set_{}", currLineLabel, localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", isEmpty, doUnset, doSet) << Symbols::LF;
        out << doUnset << ":" << Symbols::LF;
        out << std::format("  call i32 @unsetenv(ptr {})", nbuf) << Symbols::LF;
        out << std::format("  br label %{}", nextLabel) << Symbols::LF; // terminates this path
        out << doSet << ":" << Symbols::LF;
        out << std::format("  call i32 @setenv(ptr {}, ptr {}, i32 1)", nbuf, valp) << Symbols::LF;
        log() << "line " << currentLine_ << " Environ -> setenv/unsetenv" << Symbols::LF;
        return;
    }
    if (isa<BeepStmt>(stmt)) {
        const std::string bell = nextTemp(); out << std::format("  {} = getelementptr inbounds [2 x i8], ptr @.bell, i64 0, i64 0", bell) << Symbols::LF;
        const std::string fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds [3 x i8], ptr @.fmt_str_sp, i64 0, i64 0", fmt) << Symbols::LF;
        out << std::format("  call i32 (ptr, ...) @printf(ptr {}, ptr {})", fmt, bell) << Symbols::LF;
        return;
    }
    if (const auto *cd = dyn_cast<ChdirStmt>(stmt)) {
        const std::string p = emitExpr(out, cd->path.get(), "");
        const std::string ir = std::format("  call i32 @chdir(ptr {})", p);
        out << ir << Symbols::LF;
        log() << "line " << currentLine_ << " ChdirStmt chdir -> " << ir << Symbols::LF;
        return;
    }
    if (isa<ClearStmt>(stmt)) {
        const auto itVB = varsBeforeLine_.find(currentLine_);
        const std::set<std::string, std::less<>> emptyVars;
        const std::set<std::string, std::less<>> &vset = (itVB == varsBeforeLine_.end()) ? emptyVars : itVB->second;
        for (const auto &v : vset) {
            auto it = varAllocaName_.find(v);
            if (it == varAllocaName_.end()) { continue; }
            resetVar(out, v);
        }
        const auto itAB = arraysBeforeLine_.find(currentLine_);
        const std::set<std::string, std::less<>> emptyArr;
        const std::set<std::string, std::less<>> &aset = (itAB == arraysBeforeLine_.end()) ? emptyArr : itAB->second;
        for (const auto &an : aset) {
            auto itLen = arrayDims_.find(an);
            if (itLen == arrayDims_.end()) { continue; }
            long long len = 1; for (int ub : itLen->second) { long long ext = (static_cast<long long>(ub) - optionBase_ + 1); if (ext < 0) ext = 0; len *= ext; }
            if (isStringArrayNameCG(an)) {
                ensureStringArrayAllocated(out, an, static_cast<int>(len));
                const std::string base = arrayAllocaName_[an];
                for (long long i = 0; i < len; ++i) {
                    const std::string elem = nextTemp();
                    out << std::format("  {} = getelementptr inbounds [{} x ptr], ptr {}, i64 0, i64 {}", elem, len, base, i) << Symbols::LF;
                    out << std::format("  store ptr null, ptr {}", elem) << Symbols::LF;
                }
            } else {
                ensureArrayAllocated(out, an, static_cast<int>(len));
                const std::string base = arrayAllocaName_[an];
                for (long long i = 0; i < len; ++i) {
                    const std::string elem = nextTemp();
                    out << std::format("  {} = getelementptr inbounds [{} x {}], ptr {}, i64 0, i64 {}", elem, len, arrayElemType(an), base, i) << Symbols::LF;
                    switch (numKindOf(an)) {
                        case NumKind::Int16: { out << std::format("  store i32 0, ptr {}", elem) << Symbols::LF; break; }
                        case NumKind::Long32: { out << std::format("  store i64 0, ptr {}", elem) << Symbols::LF; break; }
                        case NumKind::Single: { out << std::format("  store float 0.0, ptr {}", elem) << Symbols::LF; break; }
                        case NumKind::Double: default: { out << std::format("  store double 0.0, ptr {}", elem) << Symbols::LF; break; }
                    }
                }
            }
        }
        out << "  store i32 0, ptr @gwb_data_idx" << Symbols::LF;
        for (int i = 0; i < 16; ++i) {
            const std::string ep = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", ep, i) << Symbols::LF;
            const std::string fh = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fh, ep) << Symbols::LF;
            const std::string isnn = nextTemp(); out << std::format("  {} = icmp ne ptr {}, null", isnn, fh) << Symbols::LF;
            const std::string doLbl = std::format("{}_clear_close_{}", currLineLabel, ++localCounter);
            const std::string contLbl = std::format("{}_clear_cont_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", isnn, doLbl, contLbl) << Symbols::LF;
            out << doLbl << ":" << Symbols::LF;
            out << std::format("  call i32 @fclose(ptr {})", fh) << Symbols::LF;
            out << std::format("  store ptr null, ptr {}", ep) << Symbols::LF;
            out << std::format("  br label %{}", contLbl) << Symbols::LF;
            out << contLbl << ":" << Symbols::LF;
        }
        log() << "line " << currentLine_ << " ClearStmt reset state" << Symbols::LF;
        return;
    }
    if (const auto *col = dyn_cast<ColorStmt>(stmt)) {
        auto emitColor = [&](const std::unique_ptr<Expr> &e, bool isFg) {
            if (!e) { return; }
            const std::string val = emitExpr(out, e.get(), "");
            const std::string i32v = nextTemp(); out << std::format("  {} = fptosi double {} to i32", i32v, val) << Symbols::LF;
            const std::string masked = nextTemp(); out << std::format("  {} = and i32 {}, 15", masked, i32v) << Symbols::LF;
            const std::string idx64 = nextTemp(); out << std::format("  {} = sext i32 {} to i64", idx64, masked) << Symbols::LF;
            const std::string p = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x i32], ptr {}, i64 0, i64 {}", p, (isFg ? "@.sgr_fg_tbl" : "@.sgr_bg_tbl"), idx64) << Symbols::LF;
            const std::string code = nextTemp(); out << std::format("  {} = load i32, ptr {}", code, p) << Symbols::LF;
            const std::string fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds [7 x i8], ptr @.fmt_sgr, i64 0, i64 0", fmt) << Symbols::LF;
            out << std::format("  call i32 (ptr, ...) @printf(ptr {}, i32 27, i32 {})", fmt, code) << Symbols::LF;
        };
        emitColor(col->fg, true);
        emitColor(col->bg, false);
        return;
    }
    if (const auto *sc = dyn_cast<ScreenStmt>(stmt)) {
        std::string modei32 = "0";
        if (sc->mode) {
            const std::string mv = emitExpr(out, sc->mode.get(), "");
            const std::string i32v = nextTemp(); out << std::format("  {} = fptosi double {} to i32", i32v, mv) << Symbols::LF;
            modei32 = i32v;
        }
        const std::string ir = std::format("  call void @gwb_graphics_init(i32 {})", modei32);
        out << ir << Symbols::LF;
        log() << "line " << currentLine_ << " Screen init -> " << ir << Symbols::LF;
        return;
    }
    if (const auto *ci = dyn_cast<CircleStmt>(stmt)) {
        const std::string rdy = nextTemp(); out << std::format("  {} = load i1, ptr @gwb_gfx_ready", rdy) << Symbols::LF;
        const std::string doLbl = std::format("{}_circle_do{}", currLineLabel, ++localCounter);
        const std::string contLbl = std::format("{}_circle_cont{}", currLineLabel, localCounter);
        out << std::format("  br i1 {}, label %{}, label %{}", rdy, doLbl, contLbl) << Symbols::LF;
        out << doLbl << ":" << Symbols::LF;
        const std::string xv = emitExpr(out, ci->x.get(), "");
        const std::string yv = emitExpr(out, ci->y.get(), "");
        const std::string rv = emitExpr(out, ci->r.get(), "");
        std::string colori32 = "-1";
        if (ci->color) { const std::string cv = emitExpr(out, ci->color.get(), ""); const std::string ci32 = nextTemp(); out << std::format("  {} = fptosi double {} to i32", ci32, cv) << Symbols::LF; colori32 = ci32; }
        const std::string sv = ci->start ? emitExpr(out, ci->start.get(), "") : "-1.0";
        const std::string ev = ci->end ? emitExpr(out, ci->end.get(), "") : "-1.0";
        const std::string av = ci->aspect ? emitExpr(out, ci->aspect.get(), "") : "-1.0";
        const std::string stepv = ci->step ? "true" : "false";
        const std::string ir = std::format("  call void @gwb_gfx_circle(double {}, double {}, double {}, i32 {}, double {}, double {}, double {}, i1 {})", xv, yv, rv, colori32, sv, ev, av, stepv);
        out << ir << Symbols::LF;
        log() << "line " << currentLine_ << " Circle call -> " << ir << Symbols::LF;
        out << std::format("  br label %{}", contLbl) << Symbols::LF;
        out << contLbl << ":" << Symbols::LF;
        return;
    }
    (void)nextLabel;
}

} // namespace gwbasic
