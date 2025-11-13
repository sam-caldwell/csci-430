// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/DefTypeStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <cctype>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: Parser::parseDefType
 * Summary:
 *  Parse DEFSTR/DEFINT/DEFSNG/DEFDBL letter ranges.
 * Parameters:
 *  - k: Kind of DEF TYPE statement
 * Returns:
 *  - std::unique_ptr<Stmt>: DefTypeStmt with inclusive letter ranges
 */
std::unique_ptr<Stmt> Parser::parseDefType(DefTypeStmt::Kind kind) { // NOLINT(readability-function-cognitive-complexity,readability-function-size)
    std::vector<std::pair<char, char>> ranges;
    // Expect at least one letter or letter-letter; items separated by commas
    bool first = true;
    while (true) {
        if (!check(TokenType::Identifier)) {
            if (first) {
                throw ParseError("Expected letter or range after DEF* statement");
            }
            break;
        }
        char startChar = 0;
        {
            const Token& tok = peek();
            if (tok.type != TokenType::Identifier || tok.lexeme.empty()) {
                throw ParseError("Expected letter A-Z");
            }
            const char chr = static_cast<char>(std::toupper(static_cast<unsigned char>(tok.lexeme[0])));
            if (chr < 'A' || chr > 'Z') {
                throw ParseError("Expected letter A-Z");
            }
            startChar = chr;
        }
        advance();
        char endChar = startChar;
        if (match(TokenType::Minus)) {
            if (!check(TokenType::Identifier)) {
                throw ParseError("Expected letter after '-' in DEF* range");
            }
            {
                const Token& tok = peek();
                if (tok.type != TokenType::Identifier || tok.lexeme.empty()) {
                    throw ParseError("Expected letter A-Z");
                }
                const char chr = static_cast<char>(std::toupper(static_cast<unsigned char>(tok.lexeme[0])));
                if (chr < 'A' || chr > 'Z') {
                    throw ParseError("Expected letter A-Z");
                }
                endChar = chr;
            }
            advance();
            if (endChar < startChar) {
                std::swap(startChar, endChar);
            }
        }
        ranges.emplace_back(startChar, endChar);
        first = false;
        if (!match(TokenType::Comma)) {
            break;
        }
    }
    return make_node<DefTypeStmt>({0,0}, kind, std::move(ranges));
}

} // namespace gwbasic
