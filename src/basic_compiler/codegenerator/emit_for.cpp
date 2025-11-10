// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
#include <sstream>

namespace gwbasic {

/*
 * Function: emitFor
 * Summary: Emit IR for a FOR loop structure.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - fs: Parsed ForStmt node.
 *  - currLineLabel: Base label for naming emitted blocks.
 *  - localCounter: Per-line counter to uniquify labels.
 * Returns:
 *  - void
 */
void CodeGenerator::emitFor(std::ostringstream& out, const ForStmt* fs, const std::string& currLineLabel, int& localCounter) {
    std::string loopId = std::to_string(++localCounter);
    std::string condLbl = std::format("{}_for_cond{}", currLineLabel, loopId);
    std::string bodyLbl = std::format("{}_for_body{}", currLineLabel, loopId);
    std::string incLbl  = std::format("{}_for_inc{}",  currLineLabel, loopId);
    std::string endLbl  = std::format("{}_for_end{}",  currLineLabel, loopId);

    // Initialize loop variable and jump to condition
    ensureVarAllocated(out, fs->var);
    {
        std::string startReg = emitExpr(out, fs->start.get(), currLineLabel);
        storeNumberToVar(out, fs->var, startReg);
        out << std::format("  br label %{}", condLbl) << Symbols::LF;
        log() << "line " << currentLine_ << " For -> br cond" << Symbols::LF;
    }

    // Condition block
    out << condLbl << ":" << Symbols::LF;
    std::string curVal = loadVarAsDouble(out, fs->var);
    std::string endReg = emitExpr(out, fs->end.get(), currLineLabel);
    std::string stepReg = fs->step ? emitExpr(out, fs->step.get(), currLineLabel) : std::string("1.0");
    std::string cond = computeForCond(out, curVal, endReg, stepReg);
    out << std::format("  br i1 {}, label %{}, label %{}", cond, bodyLbl, endLbl) << Symbols::LF;

    // Body block
    out << bodyLbl << ":" << Symbols::LF;
    bool forTerminated = emitForBodyStatements(out, fs, currLineLabel, localCounter);

    // Increment and back-edge if loop not terminated in body
    if (!forTerminated) {
        out << std::format("  br label %{}", incLbl) << Symbols::LF;
        out << incLbl << ":" << Symbols::LF;
        std::string stepReg2 = fs->step ? emitExpr(out, fs->step.get(), currLineLabel) : std::string("1.0");
        emitForIncrement(out, fs->var, stepReg2, condLbl);
    }

    // End block
    out << endLbl << ":" << Symbols::LF;
}

} // namespace gwbasic
