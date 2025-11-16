// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Semantics.MemIoPositive
Inputs: BLOAD/BSAVE/POKE/CALL with correct argument types
Code under test: SemanticAnalyzer::analyzeStmt for BLOAD/BSAVE/POKE/CALL ABS
Expected behavior: Programs compile (no semantic errors)
*/
TEST(Semantics, MemIoPositive) {
    const char* src =
        "10 BLOAD \"f\"\n"
        "20 BLOAD \"f\", 10\n"
        "30 BSAVE \"f\", 0, 16\n"
        "40 POKE 100, 1\n"
        "50 CALL 4096\n"
        "60 END\n";
    const const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
