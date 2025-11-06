// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/token/ToString.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: Parser::consume
 * Purpose:
 *  - Require the next token to be of a given type and advance.
 * Inputs:
 *  - t: expected TokenType
 *  - what: human-friendly name for diagnostics
 * Outputs:
 *  - void (throws ParseError on mismatch)
 */
void Parser::consume(TokenType t, const std::string& what) {
    if (!check(t)) {
        std::ostringstream oss;
        oss << "Expected " << what << ", got '" << to_string(peek().type) << "' at " << peek().line << ":" << peek().col;
        throw ParseError(oss.str());
    }
    advance();
}

} // namespace gwbasic
