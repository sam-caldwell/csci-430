// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"

namespace gwbasic {

/*
 * Function: Lexer::atEnd
 * Summary:
 *  Check whether the source cursor has reached end-of-input.
 * Parameters:
 *  - none
 * Returns:
 *  - bool: true when pos_ >= src_.size()
 */
bool Lexer::atEnd() const { return pos_ >= src_.size(); }

} // namespace gwbasic

