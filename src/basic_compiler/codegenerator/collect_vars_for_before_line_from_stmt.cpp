// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/ArrayAssignStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/ReadStmt.h"
#include "basic_compiler/ast/DimStmt.h"
#include "basic_compiler/ast/WriteStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/EraseStmt.h"
#include "basic_compiler/ast/SwapStmt.h"

namespace gwbasic {
    void CodeGenerator::collectVarsForBeforeLineFromStmt(const Stmt *s, std::set<std::string, std::less<>> &vars,
                                                         std::set<std::string, std::less<>> &arrays) {
        if (!s) return;
        if (handleAssignBeforeLine(s, vars, arrays)) return;
        if (handleArrayAssignBeforeLine(s, vars, arrays)) return;
        if (handleIfBlockBeforeLine(s, vars, arrays)) return;
        if (handleIfBeforeLine(s, vars, arrays)) return;
        if (handleForBeforeLine(s, vars, arrays)) return;
        if (handleWhileBeforeLine(s, vars, arrays)) return;
        if (handlePrintBeforeLine(s, vars, arrays)) return;
        if (handleInputBeforeLine(s, vars, arrays)) return;
        if (handleReadBeforeLine(s, vars, arrays)) return;
        if (handleDimBeforeLine(s, arrays)) return;
        if (handleSwapBeforeLine(s, vars, arrays)) return;
        if (handleEraseBeforeLine(s, arrays)) return;
        if (handleWriteBeforeLine(s, vars, arrays)) return;
        if (handleOnGotoBeforeLine(s, vars, arrays)) return;
        (void)handleOnGosubBeforeLine(s, vars, arrays);
    }
} // namespace gwbasic
