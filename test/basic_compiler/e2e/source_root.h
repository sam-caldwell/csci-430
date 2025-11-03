// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once
#include <string>

#ifndef TEST_SOURCE_ROOT
#define TEST_SOURCE_ROOT "."
#endif

namespace e2e_helpers {
inline std::string sourceRoot() { return std::string(TEST_SOURCE_ROOT); }
}

