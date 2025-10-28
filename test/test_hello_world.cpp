// File: test/test_hello_world.cpp
// (c) 2025 Sam Caldwell.  All Rights Reserved.
// Purpose: GoogleTest unit tests for hello_world module.
// Purpose: Verifies returned string equals "Hello, World".
#include <gtest/gtest.h>
extern "C" {
#include "hello_world.h"
}

TEST(HelloWorldTest, ReturnsExpectedString) {
    EXPECT_STREQ("Hello, World", get_hello_world());
}

