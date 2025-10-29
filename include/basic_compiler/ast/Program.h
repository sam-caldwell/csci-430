// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <vector>
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/VarExpr.h"

namespace gwbasic {

/**
 * Type: Program
 * Purpose:
 *  - Top-level container for an entire GW-BASIC program, preserving line
 *    order and associated statements.
 * Inputs:
 *  - lines: Sequence of Line nodes, ascending by line number
 * Outputs:
 *  - Structural root consumed by the CodeGenerator
 * Theory of operation:
 *  - Parser builds and returns a Program; downstream phases traverse it to
 *    analyze and emit LLVM IR.
 */
struct Program {
    std::vector<Line> lines;
};

} // namespace gwbasic
