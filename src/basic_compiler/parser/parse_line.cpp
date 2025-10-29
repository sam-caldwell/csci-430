// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include <sstream>

namespace gwbasic {

Line Parser::parseLine() {
    /*
     * Function: Parser::parseLine
     * Inputs:
     *  - none (expects current token to be a line number)
     * Outputs:
     *  - Line: AST node with line number and parsed statements
     * Theory of operation:
     *  - Reads a leading Integer token as the line number, then parses one or
     *    more statements separated by ':' until a newline or EOF is reached.
     */
    Line line;
    if (!check(TokenType::Integer)) {
        std::ostringstream oss;
        oss << "Expected line number at " << peek().line << ":" << peek().col;
        throw ParseError(oss.str());
    }
    line.number = std::stoi(peek().lexeme);
    advance();

    while (!atEnd() && !check(TokenType::NewLine)) {
        auto st = parseStatement();
        line.statements.push_back(std::move(st));
        const auto& last = line.statements.back();
        {
            std::ostringstream m;
            m << "line " << line.number << ' ' << nodeName(last.get()) << " @ " << last->pos.line << ':' << last->pos.col;
            logSyntax(m.str());
        }
        if (match(TokenType::Colon)) continue;
        if (check(TokenType::NewLine)) break;
    }
    match(TokenType::NewLine);
    return line;
}

} // namespace gwbasic
