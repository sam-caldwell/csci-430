// File: test/hello_world/unit/test_hello_world.cpp
// (c) 2025 Sam Caldwell.  All Rights Reserved.
/*
 * Test: HelloWorldTest.ReturnsExpectedString
 * Inputs: None (direct call to API)
 * Code under test: hello_world::get_hello_world()
 * Expected behavior: Returns literal "Hello, World"
 */
#include <gtest/gtest.h>
extern "C" {
#include "hello_world/hello_world.h"
}

TEST(HelloWorldTest, ReturnsExpectedString) {
    EXPECT_STREQ("Hello, World", get_hello_world());
}

