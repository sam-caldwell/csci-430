// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/Stmt.h"
#include <format>
#include <sstream>
#include <string>

namespace gwbasic {

// RANDOMIZE [expr]
void CodeGenerator::emitLineHandleRandomize(std::ostringstream &out, const Stmt *stmt) {
    const auto *randStmt = dyn_cast<RandomizeStmt>(stmt);
    if (randStmt == nullptr) { return; }
    if (randStmt->seed) {
        const std::string val = emitExpr(out, randStmt->seed.get(), "");
        const std::string seedInt64 = nextTemp();
        out << std::format("  {} = fptosi double {} to i64", seedInt64, val) << Symbols::LF;
        out << std::format("  call void @srand48(i64 {})", seedInt64) << Symbols::LF;
    } else {
        const std::string timeVal = nextTemp();
        out << std::format("  {} = call i64 @time(ptr null)", timeVal) << Symbols::LF;
        out << std::format("  call void @srand48(i64 {})", timeVal) << Symbols::LF;
    }
}

} // namespace gwbasic
