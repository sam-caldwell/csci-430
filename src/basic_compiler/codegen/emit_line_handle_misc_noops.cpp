// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/CommonStmt.h"
#include "basic_compiler/ast/DefFnStmt.h"
#include "basic_compiler/ast/DefTypeStmt.h"
#include "basic_compiler/ast/DefUsrStmt.h"
#include "basic_compiler/ast/DeleteStmt.h"
#include "basic_compiler/ast/DimStmt.h"
#include "basic_compiler/ast/OptionBaseStmt.h"
#include "basic_compiler/ast/OptionPrintZonesStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/UnsupportedStmt.h"
#include <sstream>

namespace gwbasic {

void CodeGenerator::emitLineHandleMiscNoops(std::ostringstream &out, const Stmt *stmt) {
    if (isa<DeleteStmt>(stmt)) { return; }
    if (isa<DimStmt>(stmt)) { return; }
    if (isa<OptionBaseStmt>(stmt)) { return; }
    if (isa<OptionPrintZonesStmt>(stmt)) { return; }
    if (const auto *unsupported = dyn_cast<UnsupportedStmt>(stmt)) { (void)unsupported; return; }
    if (isa<CommonStmt>(stmt)) { return; }
    if (isa<DefTypeStmt>(stmt)) { return; }
    if (isa<DefFnStmt>(stmt)) { return; }
    if (isa<DefUsrStmt>(stmt)) { return; }
    (void)out;
}

} // namespace gwbasic
