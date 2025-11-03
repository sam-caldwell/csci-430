// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"

namespace gwbasic {

bool SemanticAnalyzer::isKnownStringFunction(const std::string& upperName) {
    return (
        upperName == "CHR$" ||
        upperName == "LEFT$" ||
        upperName == "RIGHT$" ||
        upperName == "MID$"
    );
}

} // namespace gwbasic

