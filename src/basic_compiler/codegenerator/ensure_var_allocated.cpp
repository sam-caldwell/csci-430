// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <sstream>
#include <format>

namespace gwbasic {

void CodeGenerator::ensureVarAllocated(std::ostringstream& out, const std::string& name) {
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
    if (auto it = varAllocaName_.find(name); it != varAllocaName_.end() && !it->second.empty()) return;
    std::string a = sanitizeLocal(name);
    varAllocaName_[name] = a;
    const bool isStr = isStringVarNameCG(name);
    if (isStr) {
        std::string ir1 = std::format("  {} = alloca ptr", a);
        out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " VarAllocStr(" << name << ") -> " << ir1 << Symbols::LF;
        std::string ir2 = std::format("  store ptr null, ptr {}", a);
        out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InitNullStr(" << name << ") -> " << ir2 << Symbols::LF;
    } else {
        switch (numKindOf(name)) {
            case NumKind::Int16: {
                std::string ir1 = std::format("  {} = alloca i16", a);
                out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " VarAlloc(i16 " << name << ") -> " << ir1 << Symbols::LF;
                std::string ir2 = std::format("  store i16 0, ptr {}", a);
                out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InitZero(i16 " << name << ") -> " << ir2 << Symbols::LF;
                break;
            }
            case NumKind::Long32: {
                std::string ir1 = std::format("  {} = alloca i32", a);
                out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " VarAlloc(i32 " << name << ") -> " << ir1 << Symbols::LF;
                std::string ir2 = std::format("  store i32 0, ptr {}", a);
                out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InitZero(i32 " << name << ") -> " << ir2 << Symbols::LF;
                break;
            }
            case NumKind::Single: {
                std::string ir1 = std::format("  {} = alloca float", a);
                out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " VarAlloc(float " << name << ") -> " << ir1 << Symbols::LF;
                std::string ir2 = std::format("  store float 0.0, ptr {}", a);
                out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InitZero(float " << name << ") -> " << ir2 << Symbols::LF;
                break;
            }
            case NumKind::Double: {
                std::string ir1 = std::format("  {} = alloca double", a);
                out << ir1 << Symbols::LF; log() << "line " << currentLine_ << " VarAlloc(double " << name << ") -> " << ir1 << Symbols::LF;
                std::string ir2 = std::format("  store double 0.0, ptr {}", a);
                out << ir2 << Symbols::LF; log() << "line " << currentLine_ << " InitZero(double " << name << ") -> " << ir2 << Symbols::LF;
                break;
            }
        }
    }
}

} // namespace gwbasic
