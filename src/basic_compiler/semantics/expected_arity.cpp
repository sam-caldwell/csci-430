// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"

namespace gwbasic {

int SemanticAnalyzer::expectedArity(const std::string& upperName) {
    (void)upperName; return 1;
}

} // namespace gwbasic

