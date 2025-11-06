// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/token/ToString.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/NextStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/ElseStmt.h"
#include "basic_compiler/ast/EndIfStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/WendStmt.h"
#include "basic_compiler/ast/RunStmt.h"
#include "basic_compiler/ast/CommonStmt.h"
#include "basic_compiler/ast/ChainStmt.h"
#include "basic_compiler/ast/MergeStmt.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: Parser::parseStatement
 * Inputs:
 *  - none (examines current token)
 * Outputs:
 *  - std::unique_ptr<Stmt>: Parsed statement node
 * Theory of operation:
 *  - Dispatches based on the next token to the appropriate parse method
 *    (PRINT, assignment/LET, IF, FOR, GOTO, GOSUB/RETURN, INPUT, END),
 *    building the corresponding AST node or throwing on unexpected input.
 */
std::unique_ptr<Stmt> Parser::parseStatement() {
    const Token startTok = peek();
    if (auto s = tryParseSpecialIdentifierStatement(startTok)) return s;
    if (auto s = tryParseDefFamily(startTok)) return s;
    // Also handle DEF* range directives explicitly for robustness
    if (match(TokenType::KwDefStr)) { auto n = parseDefType(DefTypeStmt::Kind::Str); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwDefInt)) { auto n = parseDefType(DefTypeStmt::Kind::Int); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwDefSng)) { auto n = parseDefType(DefTypeStmt::Kind::Sng); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwDefDbl)) { auto n = parseDefType(DefTypeStmt::Kind::Dbl); n->pos = {startTok.line, startTok.col}; return n; }
    if (auto s = tryParseGotoGosub(startTok)) return s;
    if (auto s = tryParseOtherKeywords(startTok)) return s;
    if (check(TokenType::KwLet) || check(TokenType::Identifier)) {
        auto n = parseAssignOrLet();
        n->pos = {startTok.line, startTok.col};
        return n;
    }
    std::ostringstream oss;
    oss << "Unexpected token in statement: " << to_string(peek().type) << " at " << peek().line << ":" << peek().col;
    throw ParseError(oss.str());
}

} // namespace gwbasic
