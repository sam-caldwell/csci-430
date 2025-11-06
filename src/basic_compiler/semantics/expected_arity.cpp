// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <unordered_map>

namespace gwbasic {

/*
 * Function: SemanticAnalyzer::expectedArity
 * Inputs:
 *  - upperName: Uppercased intrinsic function name
 * Outputs:
 *  - int: Expected number of arguments
 * Theory of operation:
 *  - Returns the fixed arity for recognized intrinsics; defaults to 1.
 */
int SemanticAnalyzer::expectedArity(const std::string& upperName) {
    // Concrete arity table for supported intrinsics (names are expected uppercase)
    // Note: Functions with optional parameters (e.g., SCREEN, MID$) are validated
    //       explicitly in analyzeExpr(); here we record their minimum arity.
    static const std::unordered_map<std::string, int> kArity = {
        // Numeric intrinsics (arity = 1)
        {"SQR",   1},
        {"SQRT",  1},
        {"ABS",   1},
        {"SIN",   1},
        {"COS",   1},
        {"TAN",   1},
        {"ATN",   1},
        {"LOG",   1},
        {"EXP",   1},
        {"INT",   1},
        {"FIX",   1},
        {"SGN",   1},
        {"CINT",  1},
        {"CSNG",  1},
        {"CDBL",  1},
        {"RND",   1},
        {"PEEK",  1},
        {"USR",   1},
        {"ASC",   1},
        {"VAL",   1},
        {"LEN",   1},
        // INSTR([start,] s$, sub$) — minimum required arity is 2
        {"INSTR", 2},
        // SCREEN(row, col [, z]) — minimum required arity is 2
        {"SCREEN", 2},
        // String intrinsics
        {"CHR$",   1},
        {"LEFT$",  2},
        {"RIGHT$", 2},
        // MID$(s, start [, len]) — minimum required arity is 2
        {"MID$",   2},
    };

    auto it = kArity.find(upperName);
    if (it != kArity.end()) return it->second;
    // Default assumption for unlisted intrinsics is unary
    return 1;
}

} // namespace gwbasic
