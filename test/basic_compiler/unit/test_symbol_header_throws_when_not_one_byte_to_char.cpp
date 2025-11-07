// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <stdexcept>
#include "basic_compiler/Symbol.h"

using namespace gwbasic;

TEST(SymbolHeader, ThrowsWhenNotOneByteToChar) {
    Symbol two("<>");
    EXPECT_THROW({ (void)two.to_char(); }, std::logic_error);
}

