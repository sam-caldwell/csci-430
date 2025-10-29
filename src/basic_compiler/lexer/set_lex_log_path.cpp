// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <fstream>

namespace gwbasic {

/*
 * Function: Lexer::setLexLogPath
 * Inputs:
 *  - path: Filesystem path for the lexical analysis log output
 * Outputs:
 *  - void
 * Theory of operation:
 *  - Opens/truncates the specified file and enables token logging; each
 *    token produced during tokenize() is appended to this file.
 */
void Lexer::setLexLogPath(const std::string& path) {
    if (lexLog_.is_open()) lexLog_.close();
    lexLog_.open(path, std::ios::out | std::ios::trunc);
    lexLogEnabled_ = lexLog_.is_open();
}

} // namespace gwbasic

