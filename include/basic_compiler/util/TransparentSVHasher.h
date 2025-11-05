// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <string_view>
#include <functional>

namespace gwbasic {

/**
 * Struct: TransparentSVHasher
 * Purpose:
 *  - Provide a heterogeneous, transparent hasher suitable for associative
 *    containers keyed by std::string while allowing lookups by
 *    std::string_view and const char* without constructing a std::string.
 * Inputs:
 *  - Accepts std::string_view, std::string, and const char* for hashing.
 * Outputs:
 *  - size_t: Hash value computed via std::hash<std::string_view>.
 * Notes:
 *  - Include this in unordered_map/unordered_set as the Hash template
 *    parameter, paired with std::equal_to<> for transparent equality.
 *  - Example:
 *      std::unordered_map<std::string, int, TransparentSVHasher, std::equal_to<>> m;
 *      m["key"] = 1;
 *      auto it = m.find(std::string_view{"key"}); // no allocation
 */
struct TransparentSVHasher {
    using is_transparent = void; // enables heterogeneous lookup in associative containers

    size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }
    size_t operator()(const std::string& s) const noexcept {
        return std::hash<std::string_view>{}(s);
    }
    size_t operator()(const char* s) const noexcept {
        return std::hash<std::string_view>{}(s);
    }
};

} // namespace gwbasic

