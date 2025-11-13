// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
 * File: emit_var_expr.cpp
 * Purpose: Implement CodeGenerator::emitVarExpr (VarExpr lowering).
 */
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/VarExpr.h"
#include <cctype>
#include <format>
#include <sstream>
#include <string>

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
// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity)
std::string CodeGenerator::emitVarExpr(std::ostringstream& out, const VarExpr* var_expr) {
    if (std::string bound; lookupBinding(var_expr->name, bound)) { return bound; }
    {
        std::string upperName = var_expr->name;
        for (auto &ch1 : upperName) { ch1 = static_cast<char>(std::toupper(static_cast<unsigned char>(ch1))); }
        if (upperName == "INKEY$") {
            const std::string emptyPtr = nextTemp();
            out << std::format("  {} = getelementptr inbounds [1 x i8], ptr @.str_empty, i64 0, i64 0", emptyPtr) << Symbols::LF;
            log() << "line " << currentLine_ << " VarExpr(INKEY$) -> empty string" << Symbols::LF;
            return emptyPtr;
        }
        if (upperName == "DATE$") {
            const std::string timeVal = nextTemp(); out << std::format("  {} = call i64 @time(ptr null)", timeVal) << Symbols::LF;
            const std::string timePtr = nextTemp(); out << std::format("  {} = alloca i64", timePtr) << Symbols::LF;
            out << std::format("  store i64 {}, ptr {}", timeVal, timePtr) << Symbols::LF;
            const std::string tmPtr = nextTemp(); out << std::format("  {} = call ptr @localtime(ptr {})", tmPtr, timePtr) << Symbols::LF;
            const std::string sbufPtr = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbufPtr) << Symbols::LF;
            const std::string dateFmt = nextTemp(); out << std::format("  {} = getelementptr inbounds [9 x i8], ptr @.fmt_date, i64 0, i64 0", dateFmt) << Symbols::LF;
            const std::string written = nextTemp(); out << std::format("  {} = call i64 @strftime(ptr {}, i64 256, ptr {}, ptr {})", written, sbufPtr, dateFmt, tmPtr) << Symbols::LF;
            const std::string allocSize = nextTemp(); out << std::format("  {} = add i64 {}, 1", allocSize, written) << Symbols::LF;
            const std::string memPtr = nextTemp(); out << std::format("  {} = call ptr @malloc(i64 {})", memPtr, allocSize) << Symbols::LF;
            out << std::format("  call ptr @strcpy(ptr {}, ptr {})", memPtr, sbufPtr) << Symbols::LF;
            log() << "line " << currentLine_ << " VarExpr(DATE$) -> strftime" << Symbols::LF;
            return memPtr;
        }
        if (upperName == "TIME$") {
            const std::string timeVal = nextTemp(); out << std::format("  {} = call i64 @time(ptr null)", timeVal) << Symbols::LF;
            const std::string timePtr = nextTemp(); out << std::format("  {} = alloca i64", timePtr) << Symbols::LF;
            out << std::format("  store i64 {}, ptr {}", timeVal, timePtr) << Symbols::LF;
            const std::string tmPtr = nextTemp(); out << std::format("  {} = call ptr @localtime(ptr {})", tmPtr, timePtr) << Symbols::LF;
            const std::string sbufPtr = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_sbuf, i64 0, i64 0", sbufPtr) << Symbols::LF;
            const std::string timeFmt = nextTemp(); out << std::format("  {} = getelementptr inbounds [9 x i8], ptr @.fmt_time, i64 0, i64 0", timeFmt) << Symbols::LF;
            const std::string written = nextTemp(); out << std::format("  {} = call i64 @strftime(ptr {}, i64 256, ptr {}, ptr {})", written, sbufPtr, timeFmt, tmPtr) << Symbols::LF;
            const std::string allocSize = nextTemp(); out << std::format("  {} = add i64 {}, 1", allocSize, written) << Symbols::LF;
            const std::string memPtr = nextTemp(); out << std::format("  {} = call ptr @malloc(i64 {})", memPtr, allocSize) << Symbols::LF;
            out << std::format("  call ptr @strcpy(ptr {}, ptr {})", memPtr, sbufPtr) << Symbols::LF;
            log() << "line " << currentLine_ << " VarExpr(TIME$) -> strftime" << Symbols::LF;
            return memPtr;
        }
    }
    ensureVarAllocated(out, var_expr->name);
    std::string allocaPtr = varAllocaName_[var_expr->name];
    std::string result = nextTemp();

    if (isStringVarNameCG(var_expr->name)) {
        const std::string loadPtrLine = std::format("  {} = load ptr, ptr {}", result, allocaPtr);
        out << loadPtrLine << Symbols::LF;
        std::string safe = nextTemp();
        { const std::string irLine = std::format("  {} = call ptr @gwb_safe_str(ptr {})", safe, result); out << irLine << Symbols::LF; }
        result = safe;
        log() << "line " << currentLine_ << " VarExpr$ -> load+safe" << Symbols::LF;
    } else {
        switch (numKindOf(var_expr->name)) {
            case NumKind::Int16: {
                const std::string tmpVal = nextTemp();
                { const std::string irLine = std::format("  {} = load i16, ptr {}", tmpVal, allocaPtr); out << irLine << Symbols::LF; }
                { const std::string irLine = std::format("  {} = sitofp i16 {} to double", result, tmpVal); out << irLine << Symbols::LF; }
                break;
            }
            case NumKind::Long32: {
                const std::string tmpVal = nextTemp();
                { const std::string irLine = std::format("  {} = load i32, ptr {}", tmpVal, allocaPtr); out << irLine << Symbols::LF; }
                { const std::string irLine = std::format("  {} = sitofp i32 {} to double", result, tmpVal); out << irLine << Symbols::LF; }
                break;
            }
            case NumKind::Single: {
                const std::string tmpVal = nextTemp();
                { const std::string irLine = std::format("  {} = load float, ptr {}", tmpVal, allocaPtr); out << irLine << Symbols::LF; }
                { const std::string irLine = std::format("  {} = fpext float {} to double", result, tmpVal); out << irLine << Symbols::LF; }
                break;
            }
            case NumKind::Double: {
                const std::string irLine = std::format("  {} = load double, ptr {}", result, allocaPtr);
                out << irLine << Symbols::LF;
                break;
            }
        }
        log() << "line " << currentLine_ << " VarExpr -> load/convert to double" << Symbols::LF;
    }
    return result;
}
