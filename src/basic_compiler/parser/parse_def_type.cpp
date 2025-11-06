// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/DefTypeStmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/token/ToString.h"
#include <cctype>
#include <sstream>

namespace gwbasic {

/*
 * Function: Parser::parseDefType
 * Purpose:
 *  - Parse DEFSTR/DEFINT/DEFSNG/DEFDBL letter ranges.
 * Inputs:
 *  - k: Kind of DEF TYPE statement
 * Outputs:
 *  - DefTypeStmt: list of inclusive letter ranges (A..Z)
 */
std::unique_ptr<Stmt> Parser::parseDefType(DefTypeStmt::Kind k) {
    std::vector<std::pair<char,char>> ranges;
    // Expect at least one letter or letter-letter; items separated by commas
    bool first = true;
    while (true) {
        if (!check(TokenType::Identifier)) {
            if (first) throw ParseError("Expected letter or range after DEF* statement");
            break;
        }
        char a;
        {
            const Token& t = peek();
            if (t.type != TokenType::Identifier || t.lexeme.empty()) throw ParseError("Expected letter A-Z");
            char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(t.lexeme[0])));
            if (ch < 'A' || ch > 'Z') throw ParseError("Expected letter A-Z");
            a = ch;
        }
        advance();
        char b = a;
        if (match(TokenType::Minus)) {
            if (!check(TokenType::Identifier)) throw ParseError("Expected letter after '-' in DEF* range");
            {
                const Token& t = peek();
                if (t.type != TokenType::Identifier || t.lexeme.empty()) throw ParseError("Expected letter A-Z");
                char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(t.lexeme[0])));
                if (ch < 'A' || ch > 'Z') throw ParseError("Expected letter A-Z");
                b = ch;
            }
            advance();
            if (b < a) std::swap(a, b);
        }
        ranges.emplace_back(a, b);
        first = false;
        if (!match(TokenType::Comma)) break;
    }
    return make_node<DefTypeStmt>({0,0}, k, std::move(ranges));
}

} // namespace gwbasic
