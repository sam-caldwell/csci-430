// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/token/Token.h"
#include <vector>

namespace gwbasic {

/*
 * Function: Lexer::emitToken
 * Summary:
 *  Append a token to the output vector and log it.
 * Parameters:
 *  - out: Destination token vector
 *  - token: Token to emit
 * Returns:
 *  - void
 */
void Lexer::emitToken(std::vector<Token>& out, const Token& token) {
    out.emplace_back(token);
    logToken(token);
    if (gMetrics != nullptr) {
        gMetrics->incToken();
    }
}

} // namespace gwbasic
