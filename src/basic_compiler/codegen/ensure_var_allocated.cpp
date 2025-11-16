// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>

// NOLINTBEGIN(llvmlibc-callee-namespace, llvmlibc-implementation-in-namespace)

namespace gwbasic {

/*
 * Function: ensureVarAllocated
 * Summary: Ensure a local allocation exists for a given variable name.
 * Parameters:
 *  - out: IR stream (insertion point).
 *  - name: Variable identifier.
 * Returns:
 *  - void (may emit an alloca and zero-initialize)
 */
void CodeGenerator::ensureVarAllocated(std::ostringstream& out, const std::string& name) { // NOLINT(readability-function-size)
    if (auto foundIt = varAllocaName_.find(name);
        foundIt != varAllocaName_.end() && !foundIt->second.empty()) {
        return;
    }
    std::string allocName = sanitizeLocal(name);
    varAllocaName_[name] = allocName;
    const bool isStr = isStringVarNameCG(name);
    if (isStr) {
        const std::string ir1 = std::format("  {} = alloca ptr", allocName);
        out << ir1 << Symbols::LF;
        const std::string ir2 = std::format("  store ptr null, ptr {}", allocName);
        out << ir2 << Symbols::LF;
    } else {
        switch (numKindOf(name)) {
            case NumKind::Int16: {
                const std::string ir1 = std::format("  {} = alloca i16", allocName);
                out << ir1 << Symbols::LF;
                const std::string ir2 = std::format("  store i16 0, ptr {}", allocName);
                out << ir2 << Symbols::LF;
                break;
            }
            case NumKind::Long32: {
                const std::string ir1 = std::format("  {} = alloca i32", allocName);
                out << ir1 << Symbols::LF;
                const std::string ir2 = std::format("  store i32 0, ptr {}", allocName);
                out << ir2 << Symbols::LF;
                break;
            }
            case NumKind::Single: {
                const std::string ir1 = std::format("  {} = alloca float", allocName);
                out << ir1 << Symbols::LF;
                const std::string ir2 = std::format("  store float 0.0, ptr {}", allocName);
                out << ir2 << Symbols::LF;
                break;
            }
            case NumKind::Double: {
                const std::string ir1 = std::format("  {} = alloca double", allocName);
                out << ir1 << Symbols::LF;
                const std::string ir2 = std::format("  store double 0.0, ptr {}", allocName);
                out << ir2 << Symbols::LF;
                break;
            }
        }
    }
}

} // namespace gwbasic

// NOLINTEND(llvmlibc-callee-namespace, llvmlibc-implementation-in-namespace)
