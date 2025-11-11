// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_COMPILER_FILEOPENERROR_H
#define BASIC_COMPILER_COMPILER_FILEOPENERROR_H

#include <stdexcept>
#include <string_view>

namespace gwbasic {

/**
 * Class: FileOpenError
 * Purpose:
 *  - Dedicated exception indicating a failure to open a source file for
 *    compilation-related processing (lexing/parsing/compiling).
 * Usage:
 *  - Thrown from compiler helpers when std::ifstream fails to open the
 *    requested path; catch as gwbasic::FileOpenError for fine-grained
 *    error handling in callers or tests.
 * Construction:
 *  - Accepts the attempted path and formats a consistent message.
 */
class FileOpenError final : public std::runtime_error {
public:
    explicit FileOpenError(std::string_view path);
};

} // namespace gwbasic

#endif // BASIC_COMPILER_COMPILER_FILEOPENERROR_H
