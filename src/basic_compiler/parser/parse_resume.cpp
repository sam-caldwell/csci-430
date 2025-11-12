// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/make_node.h"
#include "basic_compiler/ast/ResumeStmt.h"

namespace gwbasic {

/*
 * Function: Parser::parseResume
 * Summary:
 *  Parse RESUME with optional 0, NEXT, or specific line.
 * Parameters:
 *  - none
 * Returns:
 *  - std::unique_ptr<Stmt>: ResumeStmt indicating resume behavior
 */
std::unique_ptr<Stmt> Parser::parseResume() {
    // Current token is KwResume already consumed by caller
    if (check(TokenType::KwNext)) { advance(); return make_node<ResumeStmt>({0,0}, ResumeStmt::Kind::Next); }
    if (check(TokenType::Integer)) {
        int ln = std::stoi(peek().lexeme);
        advance();
        if (ln == 0) { return make_node<ResumeStmt>({0,0}, ResumeStmt::Kind::Reexecute); }
        return make_node<ResumeStmt>({0,0}, ResumeStmt::Kind::Line, ln);
    }
    // No argument => RESUME (re-execute)
    return make_node<ResumeStmt>({0,0}, ResumeStmt::Kind::Reexecute);
}

} // namespace gwbasic
