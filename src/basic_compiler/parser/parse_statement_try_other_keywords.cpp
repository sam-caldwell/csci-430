// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include <optional>
#include <string>
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/ElseStmt.h"
#include "basic_compiler/ast/EndIfStmt.h"
#include "basic_compiler/ast/WendStmt.h"
#include "basic_compiler/ast/NextStmt.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include "basic_compiler/ast/RandomizeStmt.h"

namespace gwbasic {

/*
 * Function: Parser::tryParseOtherKeywords
 * Inputs:
 *  - startTok: Starting token for pos metadata
 * Outputs:
 *  - Parsed statement node for a wide range of keyword-led statements;
 *    nullptr if the current token is not a recognized keyword or case here.
 * Theory of operation:
 *  - Groups the common single-keyword dispatch cases used by parseStatement
 *    to keep the top-level function concise.
 */
std::unique_ptr<Stmt> Parser::tryParseOtherKeywords(const Token& startTok) {
    // Primary I/O and control
    if (match(TokenType::KwPrint)) { auto n = parsePrint(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwIf)) { auto n = parseIf(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwWhile)) { auto n = parseWhile(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwFor)) { auto n = parseFor(); n->pos = {startTok.line, startTok.col}; return n; }

    // Declarations / File I/O / Data
    if (match(TokenType::KwCommon)) { auto n = parseCommon(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwDim)) { auto n = parseDim(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwOpen)) { auto n = parseOpen(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwClose)) { auto n = parseClose(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwData)) { auto n = parseData(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwRead)) { auto n = parseRead(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwRestore)) { auto n = parseRestore(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwWrite)) { auto n = parseWrite(); n->pos = {startTok.line, startTok.col}; return n; }

    // DEF type ranges
    if (match(TokenType::KwDefStr)) { auto n = parseDefType(DefTypeStmt::Kind::Str); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwDefInt)) { auto n = parseDefType(DefTypeStmt::Kind::Int); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwDefSng)) { auto n = parseDefType(DefTypeStmt::Kind::Sng); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwDefDbl)) { auto n = parseDefType(DefTypeStmt::Kind::Dbl); n->pos = {startTok.line, startTok.col}; return n; }

    // System / memory / environment
    if (match(TokenType::KwOption)) { auto n = parseOptionBase(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwBload)) { auto n = parseBload(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwBsave)) { auto n = parseBsave(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwPoke)) { auto n = parsePoke(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwCall)) { auto n = parseCallAbs(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwChdir)) { auto n = parseChdir(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwColor)) { auto n = parseColor(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwLine)) { consume(TokenType::KwInput, "INPUT"); auto n = parseLineInput(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwClear)) { auto n = parseClear(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwChain)) { auto n = parseChain(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwMerge)) { auto n = parseMerge(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwRun)) { auto n = parseRun(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwStop)) { return make_node<StopStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwSystem)) { return make_node<SystemStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwOn)) {
        if (check(TokenType::KwError)) {
            auto n = parseOnErrorGoto(); n->pos = {startTok.line, startTok.col}; return n;
        } else {
            auto n = parseOnGotoGosub(); n->pos = {startTok.line, startTok.col}; return n;
        }
    }

    // Misc simple keywords
    if (match(TokenType::KwReturn)) { return make_node<ReturnStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwError)) { auto n = parseError(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwResume)) { auto n = parseResume(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwInput)) { auto n = parseInput(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwRandomize)) {
        std::unique_ptr<Expr> seed;
        if (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
            seed = parseExpression();
        }
        return make_node<RandomizeStmt>({startTok.line, startTok.col}, std::move(seed));
    }
    if (match(TokenType::KwElse)) { return make_node<ElseStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwWend)) { return make_node<WendStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwNext)) {
        std::optional<std::string> v;
        if (check(TokenType::Identifier)) { v = peek().lexeme; advance(); }
        return make_node<NextStmt>({startTok.line, startTok.col}, std::move(v));
    }
    if (match(TokenType::KwEnd)) {
        if (match(TokenType::KwIf)) return make_node<EndIfStmt>({startTok.line, startTok.col});
        return make_node<EndStmt>({startTok.line, startTok.col});
    }
    return nullptr;
}

} // namespace gwbasic
