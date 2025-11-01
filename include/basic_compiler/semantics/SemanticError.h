// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <stdexcept>

namespace gwbasic {

class SemanticError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

} // namespace gwbasic

