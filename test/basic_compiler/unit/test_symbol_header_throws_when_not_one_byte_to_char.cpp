// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <stdexcept>
#include "basic_compiler/Symbol.h"

using namespace gwbasic;

/***
Test: SymbolHeader.ThrowsWhenNotOneByteToChar
Purpose: Verify `Symbol::to_char()` is only valid for single-byte symbols.
Components Under Test: `Symbol::to_char` behavior with multi-byte symbol.
Expected Behavior: Throws `std::logic_error` when called on a two-byte symbol (e.g., "<>").
*/
TEST(SymbolHeader, ThrowsWhenNotOneByteToChar) {
    constexpr Symbol two("<>");
    EXPECT_THROW({ (void)two.to_char(); }, std::logic_error);
}
