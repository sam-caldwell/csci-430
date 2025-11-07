// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <stdexcept>
#include "basic_compiler/Symbol.h"

using namespace gwbasic;

TEST(SymbolHeader, ThrowsWhenInvalidStringView) {
    // Empty and >2 bytes should throw
    EXPECT_THROW({ Symbol bad(std::string_view("")); }, std::invalid_argument);
    EXPECT_THROW({ Symbol bad(std::string_view("ABC")); }, std::invalid_argument);
}

