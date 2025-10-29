// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

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
enum class BinaryOp {
    Add, Sub, Mul, Div,
    Eq, Ne, Lt, Le, Gt, Ge
};

} // namespace gwbasic
