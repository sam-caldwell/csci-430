// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

/*
 * Function: Parser::setSyntaxLogPath
 * Inputs:
 *  - path: Filesystem path to write syntax log entries
 * Outputs:
 *  - void (opens/rotates the log file and enables logging)
 * Theory of operation:
 *  - Closes any existing file, opens 'path' for truncation, and sets the
 *    internal flag when the file is open.
 */
void Parser::setSyntaxLogPath(const std::string& path) {
    syntaxLogger_.open(path, /*append=*/false);
    syntaxLogger_.setEnabled(true);
}

} // namespace gwbasic
