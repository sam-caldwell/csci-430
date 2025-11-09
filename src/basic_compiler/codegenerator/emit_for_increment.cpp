// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitForIncrement(std::ostringstream& out,
                                     const std::string& varName,
                                     const std::string& stepReg,
                                     const std::string& condLbl) {
    std::string vcur = loadVarAsDouble(out, varName);
    std::string vnext = nextTemp();
    out << std::format("  {} = fadd double {}, {}", vnext, vcur, stepReg) << Symbols::LF;
    storeNumberToVar(out, varName, vnext);
    out << std::format("  br label %{}", condLbl) << Symbols::LF;
}

} // namespace gwbasic

