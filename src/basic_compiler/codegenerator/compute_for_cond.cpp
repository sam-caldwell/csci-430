// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

namespace gwbasic {

/*
 * Function: computeForCond
 * Summary: Compute the FOR loop condition based on step sign.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - curVal: SSA name of current induction variable value (double).
 *  - endReg: SSA name of inclusive end bound (double).
 *  - stepReg: SSA name of loop step (double).
 * Returns:
 *  - std::string: SSA register name holding the i1 condition value.
 */
std::string CodeGenerator::computeForCond(std::ostringstream& out,
                                          const std::string& curVal,
                                          const std::string& endReg,
                                          const std::string& stepReg) {
    std::string isNeg = nextTemp();
    std::string condLe = nextTemp();
    std::string condGe = nextTemp();
    std::string cond = nextTemp();
    out << std::format("  {} = fcmp olt double {}, 0.0", isNeg, stepReg) << Symbols::LF
        << std::format("  {} = fcmp ole double {}, {}", condLe, curVal, endReg) << Symbols::LF
        << std::format("  {} = fcmp oge double {}, {}", condGe, curVal, endReg) << Symbols::LF
        << std::format("  {} = select i1 {}, i1 {}, i1 {}", cond, isNeg, condGe, condLe)
        << Symbols::LF;
    return cond;
}

} // namespace gwbasic
