// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/token/ToString.h"
#include <sstream>

namespace gwbasic {

void Parser::consume(TokenType t, const std::string& what) {
    /*
     * Function: Parser::consume
     * Inputs:
     *  - t: required token type
     *  - what: human-friendly description for diagnostics
     * Outputs:
     *  - void (advances the token cursor)
     * Theory of operation:
     *  - Verifies the current token matches the expected type; otherwise
     *    throws ParseError with context; advances on success.
     */
    if (!check(t)) {
        std::ostringstream oss;
        oss << "Expected " << what << ", got '" << to_string(peek().type) << "' at " << peek().line << ":" << peek().col;
        throw ParseError(oss.str());
    }
    advance();
}

} // namespace gwbasic
