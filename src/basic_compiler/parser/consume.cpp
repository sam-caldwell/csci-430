// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Parser.h"
#include "basic_compiler/token/ToString.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: Parser::consume
 * Summary:
 *  Require the next token to be a given type and advance.
 * Parameters:
 *  - t: Expected TokenType
 *  - what: Human-friendly name for diagnostics
 * Returns:
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
