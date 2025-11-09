// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ColorStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <utility>

namespace gwbasic {

/*
 * Function: Parser::parseColor
 * Summary:
 *  Parse COLOR [fg][,[bg][,[border]]] into a ColorStmt.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ColorStmt with optional fg/bg/border expressions
 */
std::unique_ptr<Stmt> Parser::parseColor() {
    // COLOR [fg][,[bg][,[border]]]
    std::unique_ptr<Expr> fgExpr;
    std::unique_ptr<Expr> bgExpr;
    std::unique_ptr<Expr> border;

    auto atLineEnd = [&] {
        return check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile);
    };
    auto atLineEndOrComma = [&] {
        return atLineEnd() || check(TokenType::Comma);
    };
    auto maybeParseExpr = [&]() -> std::unique_ptr<Expr> {
        if (atLineEndOrComma()) { return nullptr; }
        return parseExpression();
    };

    fgExpr = maybeParseExpr();
    const bool haveFirstSep = match(TokenType::Comma);
    if (haveFirstSep) {
        bgExpr = maybeParseExpr();
        const bool haveSecondSep = match(TokenType::Comma);
        if (haveSecondSep && !atLineEnd()) {
            border = parseExpression();
        }
    }
    return make_node<ColorStmt>({0,0}, std::move(fgExpr), std::move(bgExpr), std::move(border));
}

} // namespace gwbasic
