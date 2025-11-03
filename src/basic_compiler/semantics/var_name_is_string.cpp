// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <cctype>

namespace gwbasic {

bool SemanticAnalyzer::varNameIsString(const std::string& name) const {
    if (name.empty()) return false;
    const char last = name.back();
    if (last == '$') return true;
    if (last == '%' || last == '!' || last == '#') return false; // explicit numeric suffix overrides
    char c0 = static_cast<char>(std::toupper(static_cast<unsigned char>(name[0])));
    if (c0 < 'A' || c0 > 'Z') return false;
    return defaultKinds_[c0 - 'A'] == DefaultKind::Str;
}

} // namespace gwbasic
