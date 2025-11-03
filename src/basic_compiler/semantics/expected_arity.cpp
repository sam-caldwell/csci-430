// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"

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
    (void)upperName; return 1;
}

} // namespace gwbasic
