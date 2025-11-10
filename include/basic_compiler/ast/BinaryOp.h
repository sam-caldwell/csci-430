// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_BINARYOP_H
#define BASIC_COMPILER_AST_BINARYOP_H

#include <cstdint>

namespace gwbasic {

/**
 * Enum: BinaryOp
 * Purpose:
 *  - Enumerate binary arithmetic and comparison operators supported by
 *    the grammar and code generator.
 * Members:
 *  - Add, Sub, Mul, Div: arithmetic
 *  - Eq, Ne, Lt, Le, Gt, Ge: comparisons yielding 0.0/1.0 in codegen
 */
enum class BinaryOp : std::uint8_t {
    Add, Sub, Mul, Div, IntDiv, Mod, Pow,
    And, Or,
    Eq, Ne, Lt, Le, Gt, Ge
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_BINARYOP_H
