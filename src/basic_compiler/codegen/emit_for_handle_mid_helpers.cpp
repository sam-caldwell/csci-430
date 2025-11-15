// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/MidAssignStmt.h"
#include "basic_compiler/Symbols.h"
#include <format>
#include <sstream>

namespace gwbasic {

std::string CodeGenerator::computeMidLenI64(std::ostringstream &out,
                                            const MidAssignStmt *mid,
                                            const std::string &currLineLabel) {
    const std::string len_i64 = nextTemp();
    if (mid->len) {
        out << std::format("  {} = fptosi double {} to i64", len_i64,
                           emitExpr(out, mid->len.get(), currLineLabel))
            << Symbols::LF;
    } else {
        out << std::format("  {} = call i64 @strlen(ptr {})", len_i64,
                           emitExpr(out, mid->value.get(), currLineLabel))
            << Symbols::LF;
    }
    return len_i64;
}

} // namespace gwbasic

