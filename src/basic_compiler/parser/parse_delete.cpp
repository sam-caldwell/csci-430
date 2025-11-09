// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/DeleteStmt.h"

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
std::unique_ptr<Stmt> Parser::parseDelete() {
    auto node = make_node<DeleteStmt>({peek().line, peek().col});
    // DELETE grammar mirrors LIST ranges: start | start - end | start -
    auto parseLineRefHere = [this](int& outVal, bool& isDot)->bool{
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
    int v = 0; bool dot = false;
    if (parseLineRefHere(v, dot)) {
        node->startLine = v; node->startIsDot = dot;
        if (match(TokenType::Minus)) {
            int v2 = 0; bool dot2 = false;
            if (parseLineRefHere(v2, dot2)) {
                node->endLine = v2; node->endIsDot = dot2;
            } else {
                // Open range: start -
                node->endLine.reset();
                node->endIsDot = false;
            }
        } else {
            // Single-line delete
            node->endLine = v; node->endIsDot = dot;
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
