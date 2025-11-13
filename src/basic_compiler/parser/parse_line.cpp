// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/TokenType.h"
#include <sstream>
#include <string>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseLine
 * Summary:
 *  Parse a program line: line-number followed by ':'-separated statements.
 * Parameters:
 *  - none
 * Returns:
 *  - Line: AST node with line number and parsed statements
 */
Line Parser::parseLine() {
    Line line;
    if (!check(TokenType::Integer)) {
        std::ostringstream oss;
        oss << "Expected line number at " << peek().line << ":" << peek().col;
        throw ParseError(oss.str());
    }
    line.number = std::stoi(peek().lexeme);
    advance();

    while (!atEnd() && !check(TokenType::NewLine)) {
        auto stmt = parseStatement();
        line.statements.push_back(std::move(stmt));
        const auto& last = line.statements.back();
        syntax() << "line " << line.number << ' ' << nodeName(last.get())
                 << " @ " << last->pos.line << ':' << last->pos.col << '\n';
        if (match(TokenType::Colon)) {
            continue;
        }
        if (check(TokenType::NewLine)) {
            break;
        }
    }
    match(TokenType::NewLine);
    return line;
}

} // namespace gwbasic
