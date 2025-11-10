// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include "logger/Logger.h"
#include <ostream>

namespace gwbasic {

/*
 * Function: Lexer::log
 * Summary:
 *  Obtain the ostream used for lexical logging.
 * Parameters:
 *  - none
 * Returns:
 *  - std::ostream&: stream for lex log
 */
std::ostream& Lexer::log() { return lexLogger_->stream(); }

} // namespace gwbasic
