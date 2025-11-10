// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_SYMBOL_H
#define BASIC_COMPILER_SYMBOL_H

#include <array>
#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

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
    explicit constexpr Symbol(char chr) : buf{chr, 0}, len(1) {}

    // Construct from a string literal of length 1 or 2 (N includes null terminator).
    template <std::size_t N>
    explicit constexpr Symbol(const char (&lit)[N]) { // NOLINT(cppcoreguidelines-avoid-c-arrays)
        static_assert(N == 2 || N == 3, "Symbol literal must be 1 or 2 chars");
        if constexpr (N == 2) { buf[0] = lit[0]; len = 1; }
        else { buf[0] = lit[0]; buf[1] = lit[1]; len = 2; }
    }

    // Construct from runtime string_view of size 1 or 2.
    explicit Symbol(std::string_view view) {
        if (view.empty() || view.size() > 2) {
            throw std::invalid_argument("Symbol must be 1 or 2 bytes");
        }
        buf[0] = view[0];
        if (view.size() == 2) {
            buf[1] = view[1];
        }
        len = static_cast<unsigned char>(view.size());
    }

    // Number of bytes (1 or 2).
    [[nodiscard]] constexpr unsigned size() const noexcept { return len; }

    // True if this is a single-byte symbol.
    [[nodiscard]] constexpr bool is_one() const noexcept { return len == 1; }

    // View of the bytes (no '\0').
    [[nodiscard]] constexpr std::string_view to_string_view() const noexcept { return {buf.data(), len}; }

    // Return the single char (throws if size() != 1).
    [[nodiscard]] char to_char() const {
        if (len != 1) {
            throw std::logic_error("Symbol is not one byte");
        }
        return buf[0];
    }

    // Unchecked first byte accessor for constexpr/aliases.
    [[nodiscard]] constexpr char first() const noexcept { return buf[0]; }

    // Make a std::string copy.
    [[nodiscard]] std::string to_string() const { return std::string(buf.data(), static_cast<std::size_t>(len)); }

    // Explicit conversions
    explicit operator char() const { return to_char(); }
    explicit operator std::string_view() const noexcept { return to_string_view(); }

    // Equality with another Symbol.
    friend constexpr bool operator==(const Symbol& lhs, const Symbol& rhs) noexcept {
        return lhs.len == rhs.len && (lhs.len == 1 ? lhs.buf[0] == rhs.buf[0]
                                                   : (lhs.buf[0] == rhs.buf[0] && lhs.buf[1] == rhs.buf[1]));
    }

    // Equality with char (either order).
    friend constexpr bool operator==(const Symbol& lhs, char chr) noexcept {
        return lhs.len == 1 && lhs.buf[0] == chr;
    }
    friend constexpr bool operator==(char chr, const Symbol& rhs) noexcept {
        return rhs == chr;
    }

    // Equality with string_view (size must be 1 or 2).
    friend constexpr bool operator==(const Symbol& lhs, std::string_view view) noexcept {
        return lhs.to_string_view() == view;
    }
    friend constexpr bool operator==(std::string_view view, const Symbol& rhs) noexcept {
        return rhs == view;
    }
};

inline std::ostream& operator<<(std::ostream& ostr, const Symbol& sym) {
    return ostr << sym.to_string_view();
}

} // namespace gwbasic

#endif // BASIC_COMPILER_SYMBOL_H
