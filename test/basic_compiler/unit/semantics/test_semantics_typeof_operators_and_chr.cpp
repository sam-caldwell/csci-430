// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Semantics.TypeOf_Operators_And_CHR
Inputs: Programs using MOD, INTDIV (\\), AND, OR, POW (^), and CHR$
Code under test: Semantics::typeOf classification for operators and string intrinsic
Expected behavior: All programs compile (no semantic type errors)
*/
TEST(SemanticsTypeOf, OperatorsAndCHR) {
    const char* src =
        "10 LET A = 5 MOD 2\n"
        "20 LET B% = 6 \\ 2\n"
        "30 LET C = 1 AND 0\n"
        "40 LET D = 1 OR 0\n"
        "50 LET E = 2 ^ 3\n"
        "60 PRINT CHR$(65)+\"X\"\n"
        "70 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
