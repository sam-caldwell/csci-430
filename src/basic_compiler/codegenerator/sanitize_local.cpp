// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
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
    auto isAllowed = [](const char chr) {
        return (chr >= 'A' && chr <= 'Z') ||
               (chr >= 'a' && chr <= 'z') ||
               (chr >= '0' && chr <= '9') ||
               (chr == '_') ||
               (chr == '.') ||
               (chr == '$');
    };

    bool allOk = true;
    for (const char chr : name) {
        if (!isAllowed(chr)) {
            allOk = false;
            break;
        }
    }
    if (allOk) {
        std::string result = "%";
        result += name;
        return result;
    }
    // Use quoted local name. Avoid quotes in name by replacing with underscore.
    std::string safe;
    safe.reserve(name.size());
    for (const char chr : name) {
        safe += (chr == '"') ? '_' : chr;
    }
    return std::format("%\"{}\"", safe);
}

} // namespace gwbasic
