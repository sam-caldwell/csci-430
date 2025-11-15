// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

namespace gwbasic {

/*
 * Function: setSemantics
 * Summary: Provide semantic analysis results to the code generator.
 * Parameters:
 *  - r: SemanticAnalyzer::Result structure with program facts.
 * Returns:
 *  - void
 */
void CodeGenerator::setSemantics(const SemanticAnalyzer::Result& result) {
    semProvided_ = true;
    semVariables_.clear();
    semVariables_.insert(result.variables.begin(), result.variables.end());
    semStrings_.clear();
    semStrings_.insert(result.stringLiterals.begin(), result.stringLiterals.end());
    semLineNumbers_ = result.lineNumbers;
    semCommonVariables_.clear();
    semCommonVariables_.insert(result.commonVariables.begin(), result.commonVariables.end());
    arrayDims_.clear();
    arrayDims_.insert(result.arrays.begin(), result.arrays.end());
    optionBase_ = result.optionBase;
    printZones_ = result.printZones;
    userFunctions_.clear();
    userFunctions_.insert(result.userFunctions.begin(), result.userFunctions.end());
    semStringVariables_.clear();
    semStringVariables_.insert(result.stringVariables.begin(), result.stringVariables.end());
    // Map numeric kinds from semantics into codegen's representation
    semNumericKinds_.clear();
    for (const auto& [name, kind] : result.numericKinds) {
        switch (kind) {
            case SemanticAnalyzer::Result::NumericKind::Int16: semNumericKinds_[name] = NumKind::Int16; break;
            case SemanticAnalyzer::Result::NumericKind::Long32: semNumericKinds_[name] = NumKind::Long32; break;
            case SemanticAnalyzer::Result::NumericKind::Single: semNumericKinds_[name] = NumKind::Single; break;
            case SemanticAnalyzer::Result::NumericKind::Double: semNumericKinds_[name] = NumKind::Double; break;
        }
    }
}

} // namespace gwbasic
