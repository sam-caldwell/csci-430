// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <initializer_list>
#include <regex>
#include <string>
#include <string_view>

namespace irtest {

// Return true if IR text contains any of the provided substrings.
inline bool irContainsAny(const std::string& ir,
                          std::initializer_list<std::string_view> needles) {
    for (auto s : needles) {
        if (ir.find(s) != std::string::npos) return true;
    }
    return false;
}

// Convenience: Compose "prefix + literal + suffix" variants and search.
inline bool irContainsAnyWithAffixes(const std::string& ir,
                                     std::string_view prefix,
                                     std::initializer_list<std::string_view> literals,
                                     std::string_view suffix) {
    for (auto lit : literals) {
        std::string needle;
        needle.reserve(prefix.size() + lit.size() + suffix.size());
        needle.append(prefix);
        needle.append(lit);
        needle.append(suffix);
        if (ir.find(needle) != std::string::npos) return true;
    }
    return false;
}

// Basic regex search helper (ECMAScript grammar).
inline bool irMatchesRegex(const std::string& ir, const std::string& pattern) {
    try {
        std::regex re(pattern);
        return std::regex_search(ir, re);
    } catch (...) {
        return false; // fall back to false on invalid patterns
    }
}

// Build a permissive regex for a decimal literal that may appear as either
// the short form (e.g., 1.9) or common double-rendered forms such as
// 1.8999999999999999 or 1.9000000000000001. For input like "12.3".
inline std::string approxLiteralRegex(std::string_view canonical) {
    std::string s(canonical);
    // Escape '.' for regex
    for (size_t pos = 0; (pos = s.find('.', pos)) != std::string::npos; pos += 2) s.replace(pos, 1, "\\.");
    // Allow either exact canonical or with trailing 9s/0..1
    // Pattern: <int>\.<frac>(?:9{6,}|0{6,}1)?
    // Keep it simple; this captures the common long representations.
    s += "(?:9{6,}|0{6,}1)?";
    return s;
}

// Regex search for prefix + approx(literal) + suffix
inline bool irContainsApproxLiteral(const std::string& ir,
                                    std::string_view prefix,
                                    std::string_view canonicalLiteral,
                                    std::string_view suffix) {
    std::string pattern;
    pattern.reserve(prefix.size() + canonicalLiteral.size() + suffix.size() + 32);
    // Escape prefix/suffix minimalistically for regex meta-characters
    auto esc = [](std::string_view t) {
        std::string out; out.reserve(t.size());
        for (char c : t) {
            if (c == '[' || c == ']' || c == '(' || c == ')' || c == '{' || c == '}' ||
                c == '.' || c == '+' || c == '*' || c == '?' || c == '^' || c == '$' ||
                c == '|' || c == '\\') out.push_back('\\');
            out.push_back(c);
        }
        return out;
    };
    pattern.append(esc(prefix));
    pattern.append(approxLiteralRegex(canonicalLiteral));
    pattern.append(esc(suffix));
    return irMatchesRegex(ir, pattern);
}

// Normalize IR by collapsing common long-tail double renderings (e.g.,
// 1.8999999999999999 -> 1.9, 1.9000000000000001 -> 1.9). This is a
// conservative, text-only normalization that only fires when the tail is
// followed by a non-digit (space, comma, paren, etc.), so it should not
// affect unrelated content.
inline std::string normalizeIrDecimals(const std::string& ir) {
    std::string out = ir;
    bool changed = true;
    // Pattern A: <int>.<frac_digit[0-8]> 9{6,} <non-digit>
    // Replace with: <int>. (<frac_digit+1>) <non-digit>
    std::regex reA(R"((\d+\.)([0-8])9{6,}([^0-9]))");
    // Pattern B: <int>.<frac*> 0{6,}1 <non-digit>
    // Replace with: <int>.<frac*> <non-digit>
    std::regex reB(R"((\d+\.\d*?)0{6,}1([^0-9]))");

    while (changed) {
        changed = false;
        std::string tmp;
        tmp.reserve(out.size());
        std::sregex_iterator it(out.begin(), out.end(), reA), end;
        size_t last = 0;
        for (; it != end; ++it) {
            const auto& m = *it;
            // Append preceding text
            tmp.append(out, last, static_cast<size_t>(m.position()) - last);
            // Group 1: integer part + '.'
            tmp.append(m[1].str());
            // Group 2: single fractional digit [0-8], increment by 1
            char d = m[2].str()[0];
            tmp.push_back(static_cast<char>(d + 1));
            // Group 3: non-digit suffix
            tmp.append(m[3].str());
            last = static_cast<size_t>(m.position() + m.length());
            changed = true;
        }
        if (changed) {
            tmp.append(out, last, std::string::npos);
            out.swap(tmp);
        }
        // Apply pattern B on updated text
        std::string tmp2;
        tmp2.reserve(out.size());
        std::sregex_iterator it2(out.begin(), out.end(), reB), end2;
        last = 0;
        bool changedB = false;
        for (; it2 != end2; ++it2) {
            const auto& m = *it2;
            tmp2.append(out, last, static_cast<size_t>(m.position()) - last);
            tmp2.append(m[1].str());
            tmp2.append(m[2].str());
            last = static_cast<size_t>(m.position() + m.length());
            changedB = true;
        }
        if (changedB) {
            tmp2.append(out, last, std::string::npos);
            out.swap(tmp2);
            changed = true;
        }
    }
    return out;
}

} // namespace irtest
