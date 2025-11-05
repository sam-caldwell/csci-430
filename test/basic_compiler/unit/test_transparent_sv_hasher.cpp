// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <unordered_map>
#include <string>
#include <string_view>
#include "basic_compiler/util/TransparentSVHasher.h"

using gwbasic::TransparentSVHasher;

TEST(TransparentSVHasher, HeterogeneousLookupWorks) {
    std::unordered_map<std::string, int, TransparentSVHasher, std::equal_to<>> m;
    m["alpha"] = 1;
    m.emplace("beta", 2);

    // lookup via string_view
    auto it1 = m.find(std::string_view{"alpha"});
    ASSERT_NE(it1, m.end());
    EXPECT_EQ(it1->second, 1);

    // lookup via const char*
    auto it2 = m.find("beta");
    ASSERT_NE(it2, m.end());
    EXPECT_EQ(it2->second, 2);

    // missing key
    auto it3 = m.find(std::string_view{"gamma"});
    EXPECT_EQ(it3, m.end());
}

