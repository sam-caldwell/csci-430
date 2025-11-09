// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/FileOpenError.h"
#include <stdexcept>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Class: FileOpenError
 * Purpose:
 *  - Encapsulate failures to open source files, composing a message that
 *    includes the path which failed to open.
 * Inputs:
 *  - path: Filesystem path that could not be opened
 */
FileOpenError::FileOpenError(const std::string_view path)
    : std::runtime_error(std::string("Unable to open input file: ").append(path)) {}

} // namespace gwbasic
