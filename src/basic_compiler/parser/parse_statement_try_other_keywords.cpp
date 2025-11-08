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
#include "basic_compiler/ast/UnsupportedStmt.h"
#include "basic_compiler/ast/ClsStmt.h"
#include "basic_compiler/ast/BeepStmt.h"

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
    if (match(TokenType::KwOption)) {
        // Dispatch OPTION BASE ... or OPTION PRINTZONES ...
        if (check(TokenType::KwBase)) { auto n = parseOptionBase(); n->pos = {startTok.line, startTok.col}; return n; }
        // Fallback to identifier-based branch
        if (check(TokenType::Identifier)) {
            std::string up = peek().lexeme; for (auto &ch : up) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            if (up == "PRINTZONES") { auto n = parseOptionPrintZones(); n->pos = {startTok.line, startTok.col}; return n; }
        }
        throw ParseError("Unknown OPTION directive");
    }
    if (match(TokenType::KwBload)) { auto n = parseBload(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwBsave)) { auto n = parseBsave(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwPoke)) { auto n = parsePoke(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwCall)) { auto n = parseCallAbs(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwChdir)) { auto n = parseChdir(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwColor)) { auto n = parseColor(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwLine)) { consume(TokenType::KwInput, "INPUT"); auto n = parseLineInput(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwClear)) { auto n = parseClear(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwSwap)) { auto n = parseSwap(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwErase)) { auto n = parseErase(); n->pos = {startTok.line, startTok.col}; return n; }
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
        std::vector<std::string> vars;
        if (check(TokenType::Identifier)) {
            vars.push_back(peek().lexeme);
            advance();
            while (match(TokenType::Comma)) {
                if (!check(TokenType::Identifier)) throw ParseError("Expected variable name after comma in NEXT");
                vars.push_back(peek().lexeme);
                advance();
            }
        }
        return make_node<NextStmt>({startTok.line, startTok.col}, std::move(vars));
    }
    if (match(TokenType::KwEnd)) {
        if (match(TokenType::KwIf)) return make_node<EndIfStmt>({startTok.line, startTok.col});
        return make_node<EndStmt>({startTok.line, startTok.col});
    }
    // Newly recognized keywords that are not yet implemented: parse as UnsupportedStmt
    auto parseUnsupported = [&](const std::string& kw)->std::unique_ptr<Stmt> {
        // Consume tokens to end-of-statement (before ':' or NEWLINE or EOF)
        while (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) advance();
        auto n = make_node<UnsupportedStmt>({startTok.line, startTok.col});
        n->keyword = kw;
        return n;
    };
    if (match(TokenType::KwFiles)) { return parseUnsupported("FILES"); }
    if (match(TokenType::KwName))  { return parseUnsupported("NAME"); }
    if (match(TokenType::KwName))  { auto n = parseName(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwKill))  { auto n = parseKill(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwMkdir)) { auto n = parseMkdir(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwRmdir)) { auto n = parseRmdir(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwWidth)) { auto n = parseWidth(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwLocate)) { auto n = parseLocate(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwCls))   { return make_node<ClsStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwPset))  { return parseUnsupported("PSET"); }
    if (match(TokenType::KwPreset)){ return parseUnsupported("PRESET"); }
    if (match(TokenType::KwPaint)) { return parseUnsupported("PAINT"); }
    if (match(TokenType::KwDraw))  { return parseUnsupported("DRAW"); }
    if (match(TokenType::KwView))  { return parseUnsupported("VIEW"); }
    if (match(TokenType::KwWindow)){ return parseUnsupported("WINDOW"); }
    if (match(TokenType::KwBeep))  { return make_node<BeepStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwSound)) { return parseUnsupported("SOUND"); }
    if (match(TokenType::KwPlay))  { return parseUnsupported("PLAY"); }
    if (match(TokenType::KwKey))   { return parseUnsupported("KEY"); }
    if (match(TokenType::KwPen))   { return parseUnsupported("PEN"); }
    if (match(TokenType::KwStrig)) { return parseUnsupported("STRIG"); }
    if (match(TokenType::KwTimer)) { return parseUnsupported("TIMER"); }
    if (match(TokenType::KwTron))  { return parseUnsupported("TRON"); }
    if (match(TokenType::KwTroff)) { return parseUnsupported("TROFF"); }
    if (match(TokenType::KwCont))  { return parseUnsupported("CONT"); }
    if (match(TokenType::KwLoad))  { return parseUnsupported("LOAD"); }
    if (match(TokenType::KwSave))  { return parseUnsupported("SAVE"); }
    if (match(TokenType::KwNew))   { return parseUnsupported("NEW"); }
    if (match(TokenType::KwDelete)){ return parseUnsupported("DELETE"); }
    if (match(TokenType::KwList))  { return parseUnsupported("LIST"); }
    if (match(TokenType::KwLlist)) { return parseUnsupported("LLIST"); }
    if (match(TokenType::KwAuto))  { return parseUnsupported("AUTO"); }
    if (match(TokenType::KwRenum)) { return parseUnsupported("RENUM"); }
    if (match(TokenType::KwEdit))  { return parseUnsupported("EDIT"); }
    if (match(TokenType::KwPcopy)) { return parseUnsupported("PCOPY"); }
    if (match(TokenType::KwReset)) { return parseUnsupported("RESET"); }
    if (match(TokenType::KwShell)) { auto n = parseShell(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwEnviron)){ auto n = parseEnviron(); n->pos = {startTok.line, startTok.col}; return n; }
    if (match(TokenType::KwOut))   { return parseUnsupported("OUT"); }
    if (match(TokenType::KwWait))  { return parseUnsupported("WAIT"); }
    return nullptr;
}

} // namespace gwbasic
