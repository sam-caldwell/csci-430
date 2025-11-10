// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_AST_SOURCEPOS_H
#define BASIC_COMPILER_AST_SOURCEPOS_H

namespace gwbasic {

/**
 * Type: SourcePos
 * Purpose:
 *  - Represent a source location (line and column) used for diagnostics
 *    and phase logging (lex/syntax/semantic/codegen).
 * Inputs:
 *  - line: 1-based source line number
 *  - col:  1-based column offset within the line
 * Outputs:
 *  - Plain aggregate used in AST nodes to trace origin of constructs
 * Theory of operation:
 *  - Attached to most AST nodes at creation time by the parser; later
 *    phases can reference it to map IR/log statements back to input.
 */
struct SourcePos {
    int line{0};
    int col{0};
};

} // namespace gwbasic

#endif // BASIC_COMPILER_AST_SOURCEPOS_H
