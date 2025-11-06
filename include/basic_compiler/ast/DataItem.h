// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>

namespace gwbasic {

/**
 * Type: DataItem
 * Purpose:
 *  - Represents one item in a DATA statement preserving whether it was
 *    lexed as a quoted string literal or a numeric literal.
 * Members:
 *  - isString: true when tokenized from a quoted string literal
 *  - text:     normalized text payload (string contents without quotes for
 *              strings; original numeric lexeme for numbers, possibly
 *              normalized by the lexer, e.g. &H.. → decimal digits)
 */
struct DataItem {
    bool isString{false};
    std::string text{};
};

} // namespace gwbasic

