// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/FileInputStmt.h"
#include "basic_compiler/ast/LineInputStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

// INPUT #n and LINE INPUT [#n]
// NOLINTNEXTLINE(readability-function-size,readability-identifier-length)
void CodeGenerator::emitLineHandleFileLineInput(std::ostringstream &out,
                                                const Stmt *stmt,
                                                const std::string &currLineLabel,
                                                int &localCounter) {
    if (const auto *fileInput = dyn_cast<FileInputStmt>(stmt)) {
        const int idx = fileInput->channel - 1;
        const std::string elemPtr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", elemPtr, idx) << Symbols::LF;
        const std::string fileHandle = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fileHandle, elemPtr) << Symbols::LF;
        for (const auto &vname : fileInput->variables) {
            ensureVarAllocated(out, vname);
            const std::string tmp = nextTemp(); out << std::format("  {} = alloca double", tmp) << Symbols::LF;
            const std::string fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr @.fmt_in, i64 0", fmt) << Symbols::LF;
            out << std::format("  call i32 (ptr, ...) @fscanf(ptr {}, ptr {}, ptr {})", fileHandle, fmt, tmp) << Symbols::LF;
            const std::string doubleVal = nextTemp(); out << std::format("  {} = load double, ptr {}", doubleVal, tmp) << Symbols::LF;
            storeNumberToVar(out, vname, doubleVal);
        }
        return;
    }
    if (const auto *lineInput = dyn_cast<LineInputStmt>(stmt)) {
        const std::string buf = nextTemp(); out << std::format("  {} = getelementptr inbounds [256 x i8], ptr @gwb_lbuf, i64 0, i64 0", buf) << Symbols::LF;
        if (lineInput->channel == 0) {
            const std::string fmt = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr @.fmt_line_in, i64 0", fmt) << Symbols::LF;
            out << std::format("  call i32 (ptr, ...) @scanf(ptr {}, ptr {})", fmt, buf) << Symbols::LF;
        } else {
            const int idx = lineInput->channel - 1;
            const std::string elemPtr = nextTemp(); out << std::format("  {} = getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 {}", elemPtr, idx) << Symbols::LF;
            const std::string fileHandle = nextTemp(); out << std::format("  {} = load ptr, ptr {}", fileHandle, elemPtr) << Symbols::LF;
            out << std::format("  call ptr @fgets(ptr {}, i32 256, ptr {})", buf, fileHandle) << Symbols::LF;
            const std::string len = nextTemp(); out << std::format("  {} = call i64 @strlen(ptr {})", len, buf) << Symbols::LF;
            const std::string gt0 = nextTemp(); out << std::format("  {} = icmp sgt i64 {}, 0", gt0, len) << Symbols::LF;
            const std::string doLbl = std::format("{}_li_do_{}", currLineLabel, ++localCounter);
            const std::string contLbl = std::format("{}_li_cont_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", gt0, doLbl, contLbl) << Symbols::LF;
            out << doLbl << ":" << Symbols::LF;
            const std::string lenMinus1 = nextTemp(); out << std::format("  {} = add i64 {}, -1", lenMinus1, len) << Symbols::LF;
            const std::string ptrChar = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", ptrChar, buf, lenMinus1) << Symbols::LF;
            const std::string charVal = nextTemp(); out << std::format("  {} = load i8, ptr {}", charVal, ptrChar) << Symbols::LF;
            const std::string islf = nextTemp(); out << std::format("  {} = icmp eq i8 {}, 10", islf, charVal) << Symbols::LF;
            const std::string endLbl = std::format("{}_li_end_{}", currLineLabel, localCounter);
            out << std::format("  br i1 {}, label %{}, label %{}", islf, endLbl, contLbl) << Symbols::LF;
            out << endLbl << ":" << Symbols::LF;
            out << std::format("  store i8 0, ptr {}", ptrChar) << Symbols::LF;
            out << std::format("  br label %{}", contLbl) << Symbols::LF;
            out << contLbl << ":" << Symbols::LF;
        }
        const std::string lenVal = nextTemp(); out << std::format("  {} = call i64 @strlen(ptr {})", lenVal, buf) << Symbols::LF;
        const std::string size = nextTemp(); out << std::format("  {} = add i64 {}, 1", size, lenVal) << Symbols::LF;
        const std::string mem = nextTemp(); out << std::format("  {} = call ptr @malloc(i64 {})", mem, size) << Symbols::LF;
        out << std::format("  call ptr @strncpy(ptr {}, ptr {}, i64 {})", mem, buf, lenVal) << Symbols::LF;
        const std::string ptrEnd = nextTemp(); out << std::format("  {} = getelementptr inbounds i8, ptr {}, i64 {}", ptrEnd, mem, lenVal) << Symbols::LF;
        out << std::format("  store i8 0, ptr {}", ptrEnd) << Symbols::LF;
        ensureVarAllocated(out, lineInput->name);
        out << std::format("  store ptr {}, ptr {}", mem, varAllocaName_[lineInput->name]) << Symbols::LF;
        (void)currLineLabel; (void)localCounter;
    }
}

} // namespace gwbasic
