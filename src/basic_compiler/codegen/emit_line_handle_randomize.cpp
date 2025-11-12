// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

// RANDOMIZE [expr]
void CodeGenerator::emitLineHandleRandomize(std::ostringstream &out, const Stmt *stmt) {
    const auto *rz = dyn_cast<RandomizeStmt>(stmt);
    if (!rz) { return; }
    if (rz->seed) {
        const std::string val = emitExpr(out, rz->seed.get(), "");
        const std::string si = nextTemp();
        out << std::format("  {} = fptosi double {} to i64", si, val) << Symbols::LF;
        out << std::format("  call void @srand48(i64 {})", si) << Symbols::LF;
    } else {
        const std::string t = nextTemp();
        out << std::format("  {} = call i64 @time(ptr null)", t) << Symbols::LF;
        out << std::format("  call void @srand48(i64 {})", t) << Symbols::LF;
    }
}

} // namespace gwbasic
