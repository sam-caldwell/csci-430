// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <string>

namespace gwbasic {

std::string CodeGenerator::sanitizeLocal(const std::string& name) {
    auto isAllowed = [](char c) -> bool {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '.' || c == '$';
    };
    bool allOk = true;
    for (char c : name) { if (!isAllowed(c)) { allOk = false; break; } }
    if (allOk) { std::string s = "%"; s += name; return s; }
    // Use quoted local name. Avoid quotes in name by replacing with underscore.
    std::string safe;
    safe.reserve(name.size());
    for (char c : name) safe += (c == '"') ? '_' : c;
    std::string s = "%\"";
    s += safe;
    s += "\"";
    return s;
}

} // namespace gwbasic

