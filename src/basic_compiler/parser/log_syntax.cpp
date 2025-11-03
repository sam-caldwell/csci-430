// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

/*
 * Function: Parser::logSyntax
 * Inputs:
 *  - msg: Line of text to append to the syntax log
 * Outputs:
 *  - void (writes to log if enabled)
 * Theory of operation:
 *  - Writes 'msg' with newline to the syntax log when the log file is open
 *    and logging is enabled.
 */
void Parser::logSyntax(const std::string& msg) {
    if (syntaxLogEnabled_ && syntaxLog_.is_open()) syntaxLog_ << msg << '\n';
}

} // namespace gwbasic
