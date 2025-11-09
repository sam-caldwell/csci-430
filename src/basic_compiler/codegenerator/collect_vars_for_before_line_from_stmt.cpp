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
        if (!s ||
            handleAssignBeforeLine(s, vars, arrays) ||
            handleArrayAssignBeforeLine(s, vars, arrays) ||
            handleIfBlockBeforeLine(s, vars, arrays) ||
            handleIfBeforeLine(s, vars, arrays) ||
            handleForBeforeLine(s, vars, arrays) ||
            handleWhileBeforeLine(s, vars, arrays) ||
            handlePrintBeforeLine(s, vars, arrays) ||
            handleInputBeforeLine(s, vars, &arrays) ||  // keep existing signature
            handleReadBeforeLine(s, vars, arrays) ||
            handleDimBeforeLine(s, arrays) ||
            handleSwapBeforeLine(s, vars, arrays) ||
            handleEraseBeforeLine(s, arrays) ||
            handleWriteBeforeLine(s, vars, arrays) ||
            handleOnGotoBeforeLine(s, vars, arrays)
        )
            return;

        (void)handleOnGosubBeforeLine(s, vars, arrays);
    }
} // namespace gwbasic
