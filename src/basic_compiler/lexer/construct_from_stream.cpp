// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include <istream>
#include <memory>
#include <sstream>

namespace gwbasic {

/*
 * Function: Lexer::Lexer
 * Summary:
 *  Construct a lexer by reading all characters from a stream.
 * Parameters:
 *  - inputStream: std::istream to read source from
 * Returns:
 *  - none
 */
Lexer::Lexer(std::istream& inputStream) {
    std::ostringstream buf;
    buf << inputStream.rdbuf();
    src_ = buf.str();
}

} // namespace gwbasic
