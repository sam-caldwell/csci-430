// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>

// NOLINTBEGIN(llvmlibc-callee-namespace, llvmlibc-implementation-in-namespace)

namespace gwbasic {

void CodeGenerator::ensureVarAllocated(std::ostringstream& out, const std::string& name) { // NOLINT(readability-function-size)
    /*
     * Function: CodeGenerator::ensureVarAllocated
     * Inputs:
     *  - out: IR stream (insertion point)
     *  - name: variable identifier
     * Outputs:
     *  - void (may emit an alloca+store 0.0)
     * Theory of operation:
     *  - Checks for an existing alloca mapping; if absent, emits an alloca
     *    of type double and zero-initializes it.
     */
    if (auto foundIt = varAllocaName_.find(name);
        foundIt != varAllocaName_.end() && !foundIt->second.empty()) {
        return;
    }
    std::string allocName = sanitizeLocal(name);
    varAllocaName_[name] = allocName;
    const bool isStr = isStringVarNameCG(name);
    if (isStr) {
        const std::string ir1 = std::format("  {} = alloca ptr", allocName);
        out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " VarAllocStr(" << name << ") -> " << ir1 << Symbols::LF;
        const std::string ir2 = std::format("  store ptr null, ptr {}", allocName);
        out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InitNullStr(" << name << ") -> " << ir2 << Symbols::LF;
    } else {
        switch (numKindOf(name)) {
            case NumKind::Int16: {
                const std::string ir1 = std::format("  {} = alloca i16", allocName);
                out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " VarAlloc(i16 " << name << ") -> " << ir1 << Symbols::LF;
                const std::string ir2 = std::format("  store i16 0, ptr {}", allocName);
                out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InitZero(i16 " << name << ") -> " << ir2 << Symbols::LF;
                break;
            }
            case NumKind::Long32: {
                const std::string ir1 = std::format("  {} = alloca i32", allocName);
                out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " VarAlloc(i32 " << name << ") -> " << ir1 << Symbols::LF;
                const std::string ir2 = std::format("  store i32 0, ptr {}", allocName);
                out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InitZero(i32 " << name << ") -> " << ir2 << Symbols::LF;
                break;
            }
            case NumKind::Single: {
                const std::string ir1 = std::format("  {} = alloca float", allocName);
                out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " VarAlloc(float " << name << ") -> " << ir1 << Symbols::LF;
                const std::string ir2 = std::format("  store float 0.0, ptr {}", allocName);
                out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InitZero(float " << name << ") -> " << ir2 << Symbols::LF;
                break;
            }
            case NumKind::Double: {
                const std::string ir1 = std::format("  {} = alloca double", allocName);
                out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " VarAlloc(double " << name << ") -> " << ir1 << Symbols::LF;
                const std::string ir2 = std::format("  store double 0.0, ptr {}", allocName);
                out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InitZero(double " << name << ") -> " << ir2 << Symbols::LF;
                break;
            }
        }
    }
}

} // namespace gwbasic

// NOLINTEND(llvmlibc-callee-namespace, llvmlibc-implementation-in-namespace)
