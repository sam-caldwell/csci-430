// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/BloadStmt.h"
#include "basic_compiler/ast/BsaveStmt.h"
#include "basic_compiler/ast/PokeStmt.h"
#include "basic_compiler/ast/CallAbsStmt.h"
#include "basic_compiler/ast/make_node.h"

namespace gwbasic {

std::unique_ptr<Stmt> Parser::parseBload() {
    if (!check(TokenType::String)) throw ParseError("Expected filename string after BLOAD");
    auto fn = parsePrimary(); // StringExpr
    std::unique_ptr<Expr> off;
    if (match(TokenType::Comma)) off = parseExpression();
    return make_node<BloadStmt>({0,0}, std::move(fn), std::move(off));
}

std::unique_ptr<Stmt> Parser::parseBsave() {
    if (!check(TokenType::String)) throw ParseError("Expected filename string after BSAVE");
    auto fn = parsePrimary();
    consume(TokenType::Comma, ",");
    auto off = parseExpression();
    consume(TokenType::Comma, ",");
    auto len = parseExpression();
    return make_node<BsaveStmt>({0,0}, std::move(fn), std::move(off), std::move(len));
}

std::unique_ptr<Stmt> Parser::parsePoke() {
    auto addr = parseExpression();
    consume(TokenType::Comma, ",");
    auto val = parseExpression();
    return make_node<PokeStmt>({0,0}, std::move(addr), std::move(val));
}

std::unique_ptr<Stmt> Parser::parseCallAbs() {
    auto addr = parseExpression();
    return make_node<CallAbsStmt>({0,0}, std::move(addr));
}

} // namespace gwbasic

