// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <string>

namespace gwbasic {

bool SemanticAnalyzer::isKnownStringFunction(const std::string& upperName) {
    return (
        upperName == "CHR$" ||
        upperName == "STR$" ||
        upperName == "STRING$" ||
        upperName == "SPACE$" ||
        upperName == "DATE$" ||
        upperName == "TIME$" ||
        upperName == "LTRIM$" ||
        upperName == "RTRIM$" ||
        upperName == "LEFT$" ||
        upperName == "RIGHT$" ||
        upperName == "MID$"
    );
}

} // namespace gwbasic
