// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/ast/BeepStmt.h"
#include "basic_compiler/ast/ClsStmt.h"
#include "basic_compiler/ast/DefTypeStmt.h"
#include "basic_compiler/ast/ElseStmt.h"
#include "basic_compiler/ast/EndIfStmt.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NextStmt.h"
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/StopStmt.h"
#include "basic_compiler/ast/SystemStmt.h"
#include "basic_compiler/ast/UnsupportedStmt.h"
#include "basic_compiler/ast/WendStmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <cctype>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace gwbasic {

/*
 * Function: Parser::tryParseOtherKeywords
 * Summary:
 *  Dispatch handlers for many keyword-led statements used by parseStatement.
 * Parameters:
 *  - startTok: Starting token for source position metadata
 * Returns:
 *  - std::unique_ptr<Stmt>: Parsed node or nullptr if no keyword matched
 */
std::unique_ptr<Stmt> Parser::tryParseOtherKeywords(const Token& startTok) { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    // Primary I/O and control
    if (match(TokenType::KwPrint)) { auto node = parsePrint(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwLprint)) { auto node = parseLprint(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwIf)) { auto node = parseIf(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwWhile)) { auto node = parseWhile(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwFor)) { auto node = parseFor(); node->pos = {startTok.line, startTok.col}; return node; }

    // Declarations / File I/O / Data
    if (match(TokenType::KwCommon)) { auto node = parseCommon(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwDim)) { auto node = parseDim(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwOpen)) { auto node = parseOpen(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwClose)) { auto node = parseClose(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwData)) { auto node = parseData(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwRead)) { auto node = parseRead(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwRestore)) { auto node = parseRestore(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwWrite)) { auto node = parseWrite(); node->pos = {startTok.line, startTok.col}; return node; }

    // DEF type ranges
    if (match(TokenType::KwDefStr)) { auto node = parseDefType(DefTypeStmt::Kind::Str); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwDefInt)) { auto node = parseDefType(DefTypeStmt::Kind::Int); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwDefSng)) { auto node = parseDefType(DefTypeStmt::Kind::Sng); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwDefDbl)) { auto node = parseDefType(DefTypeStmt::Kind::Dbl); node->pos = {startTok.line, startTok.col}; return node; }

    // System / memory / environment
    if (match(TokenType::KwOption)) {
        // Dispatch OPTION BASE ... or OPTION PRINTZONES ...
        if (check(TokenType::KwBase)) { auto node = parseOptionBase(); node->pos = {startTok.line, startTok.col}; return node; }
        // Fallback to identifier-based branch
        if (check(TokenType::Identifier)) {
            std::string upper = peek().lexeme;
            for (auto &chr : upper) {
                chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
            }
            if (upper == "PRINTZONES") { auto node = parseOptionPrintZones(); node->pos = {startTok.line, startTok.col}; return node; }
        }
        throw ParseError("Unknown OPTION directive");
    }
    if (match(TokenType::KwBload)) { auto node = parseBload(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwBsave)) { auto node = parseBsave(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwPoke)) { auto node = parsePoke(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwCall)) { auto node = parseCallAbs(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwChdir)) { auto node = parseChdir(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwFiles)) { auto node = parseFiles(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwColor)) { auto node = parseColor(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwLine)) { consume(TokenType::KwInput, "INPUT"); auto node = parseLineInput(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwClear)) { auto node = parseClear(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwSwap)) { auto node = parseSwap(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwErase)) { auto node = parseErase(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwChain)) { auto node = parseChain(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwMerge)) { auto node = parseMerge(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwRun)) { auto node = parseRun(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwStop)) { return make_node<StopStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwSystem)) { return make_node<SystemStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwOn)) {
        if (check(TokenType::KwError)) {
            auto node = parseOnErrorGoto(); node->pos = {startTok.line, startTok.col}; return node;
        }
        auto node = parseOnGotoGosub(); node->pos = {startTok.line, startTok.col}; return node;
    }

    // Misc simple keywords
    if (match(TokenType::KwReturn)) { return make_node<ReturnStmt>({startTok.line, startTok.col}); }
    if (match(TokenType::KwError)) { auto node = parseError(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwResume)) { auto node = parseResume(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwInput)) { auto node = parseInput(); node->pos = {startTok.line, startTok.col}; return node; }
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
                if (!check(TokenType::Identifier)) {
                    throw ParseError("Expected variable name after comma in NEXT");
                }
                vars.push_back(peek().lexeme);
                advance();
            }
        }
        return make_node<NextStmt>({startTok.line, startTok.col}, std::move(vars));
    }
    if (match(TokenType::KwEnd)) {
        if (match(TokenType::KwIf)) {
            return make_node<EndIfStmt>({startTok.line, startTok.col});
        }
        return make_node<EndStmt>({startTok.line, startTok.col});
    }
    // Newly recognized keywords that are not yet implemented: parse as UnsupportedStmt
    auto parseUnsupported = [&](const std::string& keyword) -> std::unique_ptr<Stmt> {
        // Consume tokens to end-of-statement (before ':' or NEWLINE or EOF)
        while (!(check(TokenType::NewLine) || check(TokenType::Colon) || check(TokenType::EndOfFile))) {
            advance();
        }
        auto node = make_node<UnsupportedStmt>({startTok.line, startTok.col});
        node->keyword = keyword;
        return node;
    };
    // fallthrough: no longer treat FILES/NAME as unsupported; proper parsers exist
    if (match(TokenType::KwName))  { auto node = parseName(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwKill))  { auto node = parseKill(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwMkdir)) { auto node = parseMkdir(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwRmdir)) { auto node = parseRmdir(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwWidth)) { auto node = parseWidth(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwLocate)) { auto node = parseLocate(); node->pos = {startTok.line, startTok.col}; return node; }
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
    if (match(TokenType::KwDelete)){ auto node = parseDelete(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwList))  { auto node = parseList(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwLlist)) { auto node = parseLlist(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwAuto))  { return parseUnsupported("AUTO"); }
    if (match(TokenType::KwRenum)) { return parseUnsupported("RENUM"); }
    if (match(TokenType::KwEdit))  { return parseUnsupported("EDIT"); }
    if (match(TokenType::KwPcopy)) { return parseUnsupported("PCOPY"); }
    if (match(TokenType::KwReset)) { return parseUnsupported("RESET"); }
    if (match(TokenType::KwShell)) { auto node = parseShell(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwEnviron)){ auto node = parseEnviron(); node->pos = {startTok.line, startTok.col}; return node; }
    if (match(TokenType::KwOut))   { return parseUnsupported("OUT"); }
    if (match(TokenType::KwWait))  { return parseUnsupported("WAIT"); }
    return nullptr;
}

} // namespace gwbasic
