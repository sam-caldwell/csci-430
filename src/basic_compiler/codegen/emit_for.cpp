// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include <format>
#include <sstream>
#include <string>

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
    void CodeGenerator::emitFor(std::ostringstream &out, const ForStmt *for_stmt, const std::string &currLineLabel,
                                int &localCounter) {
        const std::string loopId = std::to_string(++localCounter);
        const std::string condLbl = std::format("{}_for_cond{}", currLineLabel, loopId);
        const std::string bodyLbl = std::format("{}_for_body{}", currLineLabel, loopId);
        const std::string incLbl = std::format("{}_for_inc{}", currLineLabel, loopId);
        const std::string endLbl = std::format("{}_for_end{}", currLineLabel, loopId);

        // Initialize loop variable and jump to condition
        ensureVarAllocated(out, for_stmt->var);
        {
            const std::string startReg = emitExpr(out, for_stmt->start.get(), currLineLabel);
            storeNumberToVar(out, for_stmt->var, startReg);
            out << std::format("  br label %{}", condLbl) << Symbols::LF;
            log() << "line " << currentLine_ << " For -> br cond" << Symbols::LF;
        }

        // Condition block
        out << condLbl << ":" << Symbols::LF;
        const std::string curVal = loadVarAsDouble(out, for_stmt->var);
        const std::string endReg = emitExpr(out, for_stmt->end.get(), currLineLabel);
        const std::string stepReg = for_stmt->step
                                        ? emitExpr(out, for_stmt->step.get(), currLineLabel)
                                        : std::string("1.0");
        const std::string cond = computeForCond(out, curVal, endReg, stepReg);
        out << std::format("  br i1 {}, label %{}, label %{}", cond, bodyLbl, endLbl) << Symbols::LF;

        // Body block
        out << bodyLbl << ":" << Symbols::LF;

        // Increment and back-edge if loop not terminated in body
        if (const bool forTerminated = emitForBodyStatements(out, for_stmt, currLineLabel, localCounter); !
            forTerminated) {
            out << std::format("  br label %{}", incLbl) << Symbols::LF;
            out << incLbl << ":" << Symbols::LF;

            const std::string stepReg2 = for_stmt->step
                                             ? emitExpr(out, for_stmt->step.get(), currLineLabel)
                                             : std::string("1.0");

            emitForIncrement(out, for_stmt->var, stepReg2, condLbl);
        }

        // End block
        out << endLbl << ":" << Symbols::LF;
    }
} // namespace gwbasic
