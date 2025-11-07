// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <stdexcept>
#include "basic_compiler/Symbol.h"

using namespace gwbasic;

/***
Test: SymbolHeader.ThrowsWhenInvalidStringView
Purpose: Ensure `Symbol` rejects invalid string_view inputs (empty or length > 2).
Components Under Test: `Symbol` constructor taking `std::string_view`.
Expected Behavior: Throws `std::invalid_argument` for empty or 3+ byte inputs.
*/
TEST(SymbolHeader, ThrowsWhenInvalidStringView) {
    // Empty and >2 bytes should throw
    EXPECT_THROW({ Symbol bad(std::string_view("")); }, std::invalid_argument);
    EXPECT_THROW({ Symbol bad(std::string_view("ABC")); }, std::invalid_argument);
}
