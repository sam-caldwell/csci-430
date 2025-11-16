// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Semantics.DataReadWriteWalk
Inputs: DATA with mixed items; READ into scalar and array; WRITE #n with items
Code under test: SemanticAnalyzer::analyzeStmt for DATA/READ/WRITE
Expected behavior: Programs compile; semantics validates indices numeric
*/
TEST(Semantics, DataReadWriteWalk) {
    const char* src =
        "10 DATA \"X\", 3.14, 42\n"
        "20 DIM A(3)\n"
        "30 READ S$, A(1)\n"
        "40 WRITE #1, S$, A(1)\n"
        "50 END\n";
    const const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
