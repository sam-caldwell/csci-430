// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

/*
 * Function: Parser::setSyntaxLogPath
 * Summary:
 *  Enable parser syntax logging to the specified file path.
 * Parameters:
 *  - path: Filesystem path to write syntax log entries
 * Returns:
 *  - void
 */
void Parser::setSyntaxLogPath(const std::string& path) {
    syntaxLogger_.open(path, /*append=*/false);
    syntaxLogger_.setEnabled(true);
}

} // namespace gwbasic
