// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/Symbols.h"
#include <cctype>
#include <string>

namespace gwbasic {

SemanticAnalyzer::Result::NumericKind SemanticAnalyzer::numericKindOf(const std::string& name) const {
    using NK = Result::NumericKind;
    if (name.empty()) { return NK::Single; } // default fallback
    const char last = name.back();
    // Explicit suffix determines kind
    if (last == Symbols::PERCENT.first()) { return NK::Int16; }     // % -> 16-bit int
    if (last == Symbols::AMPERSAND.first()) { return NK::Long32; }  // & -> 32-bit long
    if (last == Symbols::EXCLAMATION.first()) { return NK::Single; } // ! -> single
    if (last == Symbols::HASH.first()) { return NK::Double; }        // # -> double
    // Otherwise, use DEF-type mapping by first letter
    const char firstChar = static_cast<char>(std::toupper(static_cast<unsigned char>(name[0])));
    if (firstChar >= 'A' && firstChar <= 'Z') {
        switch (defaultKinds_.at(firstChar - 'A')) { // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
            case DefaultKind::Int: return NK::Int16;
            case DefaultKind::Sng: return NK::Single;
            case DefaultKind::Dbl: return NK::Double;
            case DefaultKind::Str: /* not expected here (handled by varNameIsString) */ [[fallthrough]];
            case DefaultKind::None: break;
        }
    }
    // Default numeric kind in GW-BASIC is single precision when not otherwise specified
    return NK::Single;
}

} // namespace gwbasic
