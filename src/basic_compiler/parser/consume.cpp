// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/parser/ParseError.h"
#include "basic_compiler/token/ToString.h"
#include "basic_compiler/token/TokenType.h"
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: Parser::consume
 * Summary:
 *  Require the next token to be a given type and advance.
 * Parameters:
 *  - tokType: Expected TokenType
 *  - what: Human-friendly name for diagnostics
 * Returns:
 *  - void (throws ParseError on mismatch)
 */
void Parser::consume(TokenType tokType, const std::string& what) {
    if (!check(tokType)) {
        std::ostringstream oss;
        oss << "Expected " << what << ", got '" << to_string(peek().type) << "' at " << peek().line << ":" << peek().col;
        throw ParseError(oss.str());
    }
    advance();
}

} // namespace gwbasic
