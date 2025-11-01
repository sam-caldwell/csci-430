// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/semantics/SemanticAnalyzer.h"

namespace gwbasic {

bool SemanticAnalyzer::isKnownNumericFunction(const std::string& upperName) {
    return (
        upperName == "SQR" || upperName == "SQRT" || upperName == "ABS" ||
        upperName == "SIN" || upperName == "COS" || upperName == "TAN" ||
        upperName == "ATN" || upperName == "LOG" || upperName == "EXP" ||
        upperName == "INT" || upperName == "FIX" || upperName == "SGN" ||
        upperName == "CINT" || upperName == "CSNG" || upperName == "CDBL" ||
        upperName == "RND"
    );
}

} // namespace gwbasic

