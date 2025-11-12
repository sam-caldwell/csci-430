// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include <sstream>
#include <string>

namespace gwbasic {

// Delegate MID$ assignment in line-context to the FOR-body helper to avoid duplication.
// NOLINTNEXTLINE(readability-function-size)
void CodeGenerator::emitLineHandleMidAssign(std::ostringstream &out,
                                            const MidAssignStmt *mid,
                                            const std::string &currLineLabel,
                                            int &localCounter,
                                            int /*stmtIndex*/, // unused in the shared helper
                                            int /*lineNumber*/) { // unused in the shared helper
    emitForHandleMidAssign(out, mid, currLineLabel, localCounter);
}

} // namespace gwbasic
