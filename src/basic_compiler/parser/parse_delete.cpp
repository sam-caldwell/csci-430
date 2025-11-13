// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/DeleteStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/token/TokenType.h"
#include <memory>
#include <string>

namespace gwbasic {

/*
 * Function: Parser::parseDelete
 * Summary:
 *  Parse DELETE statement with LIST-like line range forms.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: DeleteStmt with optional start/end markers
 */
std::unique_ptr<Stmt> Parser::parseDelete() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    auto node = make_node<DeleteStmt>({peek().line, peek().col});
    // DELETE grammar mirrors LIST ranges: start | start - end | start -
    auto parseLineRefHere = [this](int& outVal, bool& isDot) -> bool {
        if (check(TokenType::Integer)) {
            outVal = std::stoi(peek().lexeme);
            advance();
            isDot = false;
            return true;
        }
        if (check(TokenType::Identifier) && peek().lexeme == ".") {
            advance();
            outVal = -1; // placeholder; resolved to current line later
            isDot = true;
            return true;
        }
        return false;
    };
    int startVal = 0;
    bool startIsDot = false;
    if (parseLineRefHere(startVal, startIsDot)) {
        node->startLine = startVal;
        node->startIsDot = startIsDot;
        if (match(TokenType::Minus)) {
            int endVal = 0;
            bool endIsDot = false;
            if (parseLineRefHere(endVal, endIsDot)) {
                node->endLine = endVal;
                node->endIsDot = endIsDot;
            } else {
                // Open range: start -
                node->endLine.reset();
                node->endIsDot = false;
            }
        } else {
            // Single-line delete
            node->endLine = startVal;
            node->endIsDot = startIsDot;
        }
    } else {
        // No explicit range: DELETE with no args -> delete all lines
        node->startLine.reset();
        node->endLine.reset();
        node->startIsDot = false;
        node->endIsDot = false;
    }
    return node;
}

} // namespace gwbasic
