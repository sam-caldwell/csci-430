// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Stmt.h"
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
    /*
     * Function: CodeGenerator::collectVarsForBeforeLineFromStmt
     * Summary:
     *  Scan a statement to collect variable names referenced prior to execution of a line.
     * Parameters:
     *  - stmt: Input statement to analyze.
     *  - vars: Output set of scalar variable names.
     *  - arrays: Output set of array variable names.
     * Returns:
     *  - void
     */
    void CodeGenerator::collectVarsForBeforeLineFromStmt(const Stmt *stmt, std::set<std::string, std::less<>> &vars,
                                                         std::set<std::string, std::less<>> &arrays) {
        if (!stmt ||
            handleAssignBeforeLine(stmt, vars, arrays) ||
            handleArrayAssignBeforeLine(stmt, vars, arrays) ||
            handleIfBlockBeforeLine(stmt, vars, arrays) ||
            handleIfBeforeLine(stmt, vars, arrays) ||
            handleForBeforeLine(stmt, vars, arrays) ||
            handleWhileBeforeLine(stmt, vars, arrays) ||
            handlePrintBeforeLine(stmt, vars, arrays) ||
            handleInputBeforeLine(stmt, vars, &arrays) ||  // keep existing signature
            handleReadBeforeLine(stmt, vars, arrays) ||
            handleDimBeforeLine(stmt, arrays) ||
            handleSwapBeforeLine(stmt, vars, arrays) ||
            handleEraseBeforeLine(stmt, arrays) ||
            handleWriteBeforeLine(stmt, vars, arrays) ||
            handleOnGotoBeforeLine(stmt, vars, arrays)
        )
            return;

        (void)handleOnGosubBeforeLine(stmt, vars, arrays);
    }
} // namespace gwbasic
