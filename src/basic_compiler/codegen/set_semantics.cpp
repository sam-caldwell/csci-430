// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"

namespace gwbasic {

/*
 * Function: setSemantics
 * Summary: Provide semantic analysis results to the code generator.
 * Parameters:
 *  - r: SemanticAnalyzer::Result structure with program facts.
 * Returns:
 *  - void
 */
void CodeGenerator::setSemantics(const SemanticAnalyzer::Result& r) {
    semProvided_ = true;
    semVariables_.clear();
    semVariables_.insert(r.variables.begin(), r.variables.end());
    semStrings_.clear();
    semStrings_.insert(r.stringLiterals.begin(), r.stringLiterals.end());
    semLineNumbers_ = r.lineNumbers;
    semCommonVariables_.clear();
    semCommonVariables_.insert(r.commonVariables.begin(), r.commonVariables.end());
    arrayDims_.clear();
    arrayDims_.insert(r.arrays.begin(), r.arrays.end());
    optionBase_ = r.optionBase;
    printZones_ = r.printZones;
    userFunctions_.clear();
    userFunctions_.insert(r.userFunctions.begin(), r.userFunctions.end());
    semStringVariables_.clear();
    semStringVariables_.insert(r.stringVariables.begin(), r.stringVariables.end());
    // Map numeric kinds from semantics into codegen's representation
    semNumericKinds_.clear();
    for (const auto& [name, kind] : r.numericKinds) {
        switch (kind) {
            case SemanticAnalyzer::Result::NumericKind::Int16: semNumericKinds_[name] = NumKind::Int16; break;
            case SemanticAnalyzer::Result::NumericKind::Long32: semNumericKinds_[name] = NumKind::Long32; break;
            case SemanticAnalyzer::Result::NumericKind::Single: semNumericKinds_[name] = NumKind::Single; break;
            case SemanticAnalyzer::Result::NumericKind::Double: semNumericKinds_[name] = NumKind::Double; break;
        }
    }
}

} // namespace gwbasic
