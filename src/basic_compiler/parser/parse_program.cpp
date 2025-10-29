// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"

namespace gwbasic {

Program Parser::parseProgram() {
    /*
     * Function: Parser::parseProgram
     * Inputs:
     *  - none (consumes internal token stream)
     * Outputs:
     *  - Program: AST containing ordered lines with statements
     * Theory of operation:
     *  - Skips leading blank lines; repeatedly parses a numbered line until
     *    EndOfFile, producing the program AST.
     */
    Program prog;
    while (!atEnd()) {
        while (match(TokenType::NewLine)) {}
        if (atEnd()) break;
        prog.lines.push_back(parseLine());
    }
    return prog;
}

} // namespace gwbasic
