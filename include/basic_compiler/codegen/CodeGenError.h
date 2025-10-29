// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <stdexcept>

namespace gwbasic {

/**
 * Class: CodeGenError
 * Purpose:
 *  - Signal errors encountered during code generation (e.g., unsupported
 *    constructs or violated invariants after parsing/analysis).
 * Inputs:
 *  - what_arg: Human-readable description of the failure.
 * Outputs:
 *  - Exception object derived from std::runtime_error.
 * Theory of operation:
 *  - Thrown by CodeGenerator methods; callers may catch to surface a clean
 *    error message while preserving process exit flow.
 */
class CodeGenError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

} // namespace gwbasic

