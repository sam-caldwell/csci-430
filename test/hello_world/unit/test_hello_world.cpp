// File: test/hello_world/unit/test_hello_world.cpp
// (c) 2025 Sam Caldwell.  All Rights Reserved.
/*
 * Test Suite: Hello World
 * Purpose: Sanity-check example C module returns the expected string.
 * Components Under Test: hello_world library function(s).
 * Expected Behavior: API returns the exact expected "Hello, world!" content.
 */
#include <gtest/gtest.h>
extern "C" {
#include "hello_world/hello_world.h"
}

TEST(HelloWorldTest, ReturnsExpectedString) {
    EXPECT_STREQ("Hello, World", get_hello_world());
}
