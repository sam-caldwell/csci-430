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
    if (isa<DeleteStmt>(stmt)) {
        log() << "line " << currentLine_ << " DeleteStmt (no-op at runtime)" << Symbols::LF; return; }
    if (isa<DimStmt>(stmt)) {
        log() << "line " << currentLine_ << " DimStmt (no-op)" << Symbols::LF; return; }
    if (isa<OptionBaseStmt>(stmt)) {
        log() << "line " << currentLine_ << " OptionBaseStmt (no-op)" << Symbols::LF; return; }
    if (isa<OptionPrintZonesStmt>(stmt)) {
        log() << "line " << currentLine_ << " OptionPrintZonesStmt (no-op)" << Symbols::LF; return; }
    if (const auto *unsupported = dyn_cast<UnsupportedStmt>(stmt)) {
        std::ostringstream msg; msg << "line " << currentLine_ << " UnsupportedStmt(" << unsupported->keyword << ") (no-op)"; log() << msg.str() << Symbols::LF; return; }
    if (isa<CommonStmt>(stmt)) { log() << "line " << currentLine_ << " CommonStmt (no-op)" << Symbols::LF; return; }
    if (isa<DefTypeStmt>(stmt)) { log() << "line " << currentLine_ << " DefTypeStmt (no-op)" << Symbols::LF; return; }
    if (isa<DefFnStmt>(stmt)) { log() << "line " << currentLine_ << " DefFnStmt (no-op)" << Symbols::LF; return; }
    if (isa<DefUsrStmt>(stmt)) { log() << "line " << currentLine_ << " DefUsrStmt (no-op)" << Symbols::LF; return; }
    (void)out;
}

} // namespace gwbasic
