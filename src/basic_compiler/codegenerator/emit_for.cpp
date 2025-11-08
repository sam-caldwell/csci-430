// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
#include <sstream>

namespace gwbasic {

/*
 * Function: CodeGenerator::emitFor
 * Inputs:
 *  - out: IR stream
 *  - fs: ForStmt node
 *  - currLineLabel: label base for naming blocks
 *  - localCounter: reference counter to make unique labels
 * Outputs:
 *  - void
 * Theory of operation:
 *  - Orchestrates the FOR loop emission: initialize the induction variable,
 *    evaluate the inclusive end-condition, branch to body or end, delegate the
 *    body emission to a helper, then perform the step increment and back-edge.
 */
void CodeGenerator::emitFor(std::ostringstream& out, const ForStmt* fs, const std::string& currLineLabel, int& localCounter) {
    std::string loopId = std::to_string(++localCounter);
    std::string condLbl = currLineLabel; condLbl += "_for_cond"; condLbl += loopId;
    std::string bodyLbl = currLineLabel; bodyLbl += "_for_body"; bodyLbl += loopId;
    std::string incLbl  = currLineLabel; incLbl  += "_for_inc";  incLbl  += loopId;
    std::string endLbl  = currLineLabel; endLbl  += "_for_end";  endLbl  += loopId;

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
