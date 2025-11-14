// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/Symbols.h"
#include <cctype>
#include <string>

namespace gwbasic {

bool SemanticAnalyzer::varNameIsString(const std::string& name) const {
    if (name.empty()) { return false; }
    const char last = name.back();
    if (last == Symbols::DOLLARSIGN.first()) { return true; }
    // explicit numeric suffix overrides
    if (last == Symbols::PERCENT.first() || last == Symbols::EXCLAMATION.first() || last == Symbols::HASH.first()) { return false; }
    const char firstChar = static_cast<char>(std::toupper(static_cast<unsigned char>(name[0])));
    if (firstChar < 'A' || firstChar > 'Z') { return false; }
    return defaultKinds_.at(firstChar - 'A') == DefaultKind::Str; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}

} // namespace gwbasic
