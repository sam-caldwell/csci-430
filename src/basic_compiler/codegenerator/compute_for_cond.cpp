// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

namespace gwbasic {

std::string CodeGenerator::computeForCond(std::ostringstream& out,
                                          const std::string& curVal,
                                          const std::string& endReg,
                                          const std::string& stepReg) {
    std::string isNeg = nextTemp();
    out << std::format("  {} = fcmp olt double {}, 0.0", isNeg, stepReg) << Symbols::LF;
    std::string condLe = nextTemp();
    out << std::format("  {} = fcmp ole double {}, {}", condLe, curVal, endReg) << Symbols::LF;
    std::string condGe = nextTemp();
    out << std::format("  {} = fcmp oge double {}, {}", condGe, curVal, endReg) << Symbols::LF;
    std::string cond = nextTemp();
    out << std::format("  {} = select i1 {}, i1 {}, i1 {}", cond, isNeg, condGe, condLe) << Symbols::LF;
    return cond;
}

} // namespace gwbasic

