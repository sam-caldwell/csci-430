// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/StringExpr.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parsePrint() {
    /*
     * Function: Parser::parsePrint
     * Inputs:
     *  - none (assumes PRINT already consumed)
     * Outputs:
     *  - PrintStmt: printing either a string literal or an expression
     * Theory of operation:
     *  - If the next token is STRING, returns a PrintStmt with StringExpr;
     *    otherwise parses an expression and returns a PrintStmt for numeric
     *    output.
     */
    if (check(TokenType::String)) {
        std::string s = peek().lexeme;
        const int l = peek().line;
        const int c = peek().col;
        advance();
        auto str = make_node<StringExpr>({l, c}, s);
        return make_node<PrintStmt>({l, c}, std::move(str));
    }
    auto expr = parseExpression();
    // Capture position before moving from the unique_ptr to avoid use-after-move
    const SourcePos p = expr->pos;
    return make_node<PrintStmt>(p, std::move(expr));
}

} // namespace gwbasic
