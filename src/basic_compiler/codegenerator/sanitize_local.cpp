// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>

namespace gwbasic {

/*
 * Function: sanitizeLocal
 * Summary: Produce a valid local identifier for LLVM IR from a name.
 * Parameters:
 *  - name: Proposed identifier.
 * Returns:
 *  - std::string: Local name with '%' and quoting if required.
 */
std::string CodeGenerator::sanitizeLocal(const std::string& name) {
    auto isAllowed = [](const char c){
        return (c >= 'A' && c <= 'Z') ||
               (c >= 'a' && c <= 'z') ||
               (c >= '0' && c <= '9') ||
                c == '_' ||
                c == '.' ||
                c == '$';
    };
    bool allOk = true;
    for (const char c : name) {
        if (!isAllowed(c)) {
            allOk = false; break;
        }
    }
    if (allOk) {
        std::string s = "%"; s += name; return s;
    }
    // Use quoted local name. Avoid quotes in name by replacing with underscore.
    std::string safe;
    safe.reserve(name.size());
    for (const char c : name)
        safe += (c == '"') ? '_' : c;
    return std::format("%\"{}\"", safe);
}

} // namespace gwbasic
