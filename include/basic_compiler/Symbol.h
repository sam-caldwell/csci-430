// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <array>
#include <string>
#include <string_view>
#include <stdexcept>
#include <ostream>

namespace gwbasic {

/**
 * Symbol: A lexer symbol that is exactly 1 or 2 ASCII bytes (e.g., "+", "==").
 *
 * Invariants:
 *  - size() is 1 or 2
 *  - bytes are stored without a trailing '\0'.
 *
 * Notes:
 *  - This is byte-oriented (ASCII). UTF-8 multibyte code points are not supported.
 */
struct Symbol {
    std::array<char, 2> buf{};
    unsigned char len{0};

    // Default constructs an empty invalid symbol (size 0). Prefer other constructors.
    constexpr Symbol() = default;

    // Construct from a single char.
    explicit constexpr Symbol(char c) : buf{c, 0}, len(1) {}

    // Construct from a string literal of length 1 or 2 (N includes null terminator).
    template <size_t N>
    explicit constexpr Symbol(const char (&lit)[N]) {
        static_assert(N == 2 || N == 3, "Symbol literal must be 1 or 2 chars");
        if constexpr (N == 2) { buf[0] = lit[0]; len = 1; }
        else { buf[0] = lit[0]; buf[1] = lit[1]; len = 2; }
    }

    // Construct from runtime string_view of size 1 or 2.
    explicit Symbol(std::string_view sv) {
        if (sv.empty() || sv.size() > 2) {
            throw std::invalid_argument("Symbol must be 1 or 2 bytes");
        }
        buf[0] = sv[0];
        if (sv.size() == 2) buf[1] = sv[1];
        len = static_cast<unsigned char>(sv.size());
    }

    // Number of bytes (1 or 2).
    [[nodiscard]] constexpr unsigned size() const noexcept { return len; }

    // True if this is a single-byte symbol.
    [[nodiscard]] constexpr bool is_one() const noexcept { return len == 1; }

    // View of the bytes (no '\0').
    [[nodiscard]] constexpr std::string_view to_string_view() const noexcept { return {buf.data(), len}; }

    // Return the single char (throws if size() != 1).
    [[nodiscard]] char to_char() const {
        if (len != 1) throw std::logic_error("Symbol is not one byte");
        return buf[0];
    }

    // Unchecked first byte accessor for constexpr/aliases.
    [[nodiscard]] constexpr char first() const noexcept { return buf[0]; }

    // Make a std::string copy.
    [[nodiscard]] std::string to_string() const { return {buf.data(), buf.data() + len}; }

    // Explicit conversions
    explicit operator char() const { return to_char(); }
    explicit operator std::string_view() const noexcept { return to_string_view(); }

    // Equality with another Symbol.
    friend constexpr bool operator==(const Symbol& a, const Symbol& b) noexcept {
        return a.len == b.len && (a.len == 1 ? a.buf[0] == b.buf[0]
                                             : (a.buf[0] == b.buf[0] && a.buf[1] == b.buf[1]));
    }

    // Equality with char (either order).
    friend constexpr bool operator==(const Symbol& a, char c) noexcept {
        return a.len == 1 && a.buf[0] == c;
    }
    friend constexpr bool operator==(char c, const Symbol& a) noexcept {
        return a == c;
    }

    // Equality with string_view (size must be 1 or 2).
    friend constexpr bool operator==(const Symbol& a, std::string_view sv) noexcept {
        return a.to_string_view() == sv;
    }
    friend constexpr bool operator==(std::string_view sv, const Symbol& a) noexcept {
        return a == sv;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Symbol& s) {
    return os << s.to_string_view();
}

} // namespace gwbasic

