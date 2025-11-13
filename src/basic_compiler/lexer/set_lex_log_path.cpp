// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "logger/Logger.h"
#include <string>

namespace gwbasic {

/*
 * Function: Lexer::setLexLogPath
 * Summary:
 *  Enable lexer token logging to the specified file path.
 * Parameters:
 *  - path: Filesystem path for the lexical analysis log output
 * Returns:
 *  - void
 */
void Lexer::setLexLogPath(const std::string& path) {
    lexLogger_->open(path, /*append=*/false);
    lexLogger_->setEnabled(true);
}

} // namespace gwbasic
