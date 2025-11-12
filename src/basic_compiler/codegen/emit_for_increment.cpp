// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>
#include <string>
#include <string_view>

namespace gwbasic {

/*
 * Function: emitForIncrement
 * Summary: Increment a FOR loop variable and branch to condition.
 * Parameters:
 *  - out: IR output stream to append to.
 *  - varName: Variable name to increment.
 *  - stepReg: SSA name of the step value (double).
 *  - condLbl: Label of the condition block to branch to.
 * Returns:
 *  - void
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void CodeGenerator::emitForIncrement(std::ostringstream& out,
                                     const std::string& varName,
                                     std::string_view stepReg,
                                     const std::string& condLbl) {
    std::string vcur = loadVarAsDouble(out, varName);
    std::string vnext = nextTemp();
    storeNumberToVar(out, varName, vnext);
    out << std::format("  {} = fadd double {}, {}", vnext, vcur, stepReg) << Symbols::LF
        << std::format("  br label %{}", condLbl) << Symbols::LF;
}

} // namespace gwbasic
