// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: emit_var_expr.cpp
 * Purpose: Implement CodeGenerator::emitVarExpr (VarExpr lowering).
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/RTTI.h"
#include <cctype>

using namespace gwbasic;

/*
 * Function: emitVarExpr
 * Summary: Emit IR for a variable expression, loading its value.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - v: Parsed VarExpr node.
 * Returns:
 *  - std::string: SSA register name or bound name holding the value.
 */
std::string CodeGenerator::emitVarExpr(std::ostringstream& out, const VarExpr* v) {
    if (std::string bound; lookupBinding(v->name, bound)) return bound;
    {
        std::string up = v->name; for (auto &ch : up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if (up == "INKEY$") {
            std::string p = nextTemp();
            { std::string ir = std::format("  {} = getelementptr inbounds [1 x i8], ptr @.str_empty, i64 0, i64 0", p); out << ir << Symbols::LF; }
            log() << "line " << currentLine_ << " VarExpr(INKEY$) -> empty string" << Symbols::LF;
            return p;
        }
        if (up == "DATE$") {
            std::string t = nextTemp(); { std::string ir = std::format("  {} = call i64 @time(ptr null)", t); out << ir << Symbols::LF; }
            std::string tp = nextTemp(); { std::string ir = std::format("  {} = alloca i64", tp); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i64 {}, ptr {}", t, tp); out << ir << Symbols::LF; }
            std::string tm = nextTemp(); { std::string ir = std::format("  {} = call ptr @localtime(ptr {})", tm, tp); out << ir << Symbols::LF; }
            std::string sbuf = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << ir << Symbols::LF; }
            std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [9 x i8], ptr @.fmt_date, i64 0, i64 0", fmt); out << ir << Symbols::LF; }
            std::string n = nextTemp(); { std::string ir = std::format("  {} = call i64 @strftime(ptr {}, i64 256, ptr {}, ptr {})", n, sbuf, fmt, tm); out << ir << Symbols::LF; }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, n); out << ir << Symbols::LF; }
            std::string mem = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", mem, size); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strcpy(ptr {}, ptr {})", mem, sbuf); out << ir << Symbols::LF; }
            log() << "line " << currentLine_ << " VarExpr(DATE$) -> strftime" << Symbols::LF;
            return mem;
        }
        if (up == "TIME$") {
            std::string t = nextTemp(); { std::string ir = std::format("  {} = call i64 @time(ptr null)", t); out << ir << Symbols::LF; }
            std::string tp = nextTemp(); { std::string ir = std::format("  {} = alloca i64", tp); out << ir << Symbols::LF; }
            { std::string ir = std::format("  store i64 {}, ptr {}", t, tp); out << ir << Symbols::LF; }
            std::string tm = nextTemp(); { std::string ir = std::format("  {} = call ptr @localtime(ptr {})", tm, tp); out << ir << Symbols::LF; }
            std::string sbuf = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbuf); out << ir << Symbols::LF; }
            std::string fmt = nextTemp(); { std::string ir = std::format("  {} = getelementptr inbounds [9 x i8], ptr @.fmt_time, i64 0, i64 0", fmt); out << ir << Symbols::LF; }
            std::string n = nextTemp(); { std::string ir = std::format("  {} = call i64 @strftime(ptr {}, i64 256, ptr {}, ptr {})", n, sbuf, fmt, tm); out << ir << Symbols::LF; }
            std::string size = nextTemp(); { std::string ir = std::format("  {} = add i64 {}, 1", size, n); out << ir << Symbols::LF; }
            std::string mem = nextTemp(); { std::string ir = std::format("  {} = call ptr @malloc(i64 {})", mem, size); out << ir << Symbols::LF; }
            { std::string ir = std::format("  call ptr @strcpy(ptr {}, ptr {})", mem, sbuf); out << ir << Symbols::LF; }
            log() << "line " << currentLine_ << " VarExpr(TIME$) -> strftime" << Symbols::LF;
            return mem;
        }
    }
    ensureVarAllocated(out, v->name);
    std::string a = varAllocaName_[v->name];
    std::string r = nextTemp();

    if (isStringVarNameCG(v->name)) {
        std::string ld = std::format("  {} = load ptr, ptr {}", r, a);
        out << ld << Symbols::LF;
        std::string safe = nextTemp();
        { std::string ir = std::format("  {} = call ptr @gwb_safe_str(ptr {})", safe, r); out << ir << Symbols::LF; }
        r = safe;
        log() << "line " << currentLine_ << " VarExpr$ -> load+safe" << Symbols::LF;
    } else {
        switch (numKindOf(v->name)) {
            case NumKind::Int16: {
                std::string l = nextTemp();
                { std::string ir = std::format("  {} = load i16, ptr {}", l, a); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = sitofp i16 {} to double", r, l); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Long32: {
                std::string l = nextTemp();
                { std::string ir = std::format("  {} = load i32, ptr {}", l, a); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = sitofp i32 {} to double", r, l); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Single: {
                std::string l = nextTemp();
                { std::string ir = std::format("  {} = load float, ptr {}", l, a); out << ir << Symbols::LF; }
                { std::string ir = std::format("  {} = fpext float {} to double", r, l); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Double: {
                std::string ir = std::format("  {} = load double, ptr {}", r, a);
                out << ir << Symbols::LF;
                break;
            }
        }
        log() << "line " << currentLine_ << " VarExpr -> load/convert to double" << Symbols::LF;
    }
    return r;
}
