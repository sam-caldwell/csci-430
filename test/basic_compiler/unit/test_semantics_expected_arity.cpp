// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

TEST(SemanticsExpectedArity, KnownIntrinsics) {
    auto A = SemanticAnalyzerArityAccessorForTests::expectedArity;
    // Unary numeric
    EXPECT_EQ(A("ABS"), 1);
    EXPECT_EQ(A("SIN"), 1);
    EXPECT_EQ(A("CINT"), 1);
    EXPECT_EQ(A("VAL"), 1);
    EXPECT_EQ(A("LEN"), 1);
    // String/numeric
    EXPECT_EQ(A("CHR$"), 1);
    EXPECT_EQ(A("LEFT$"), 2);
    EXPECT_EQ(A("RIGHT$"), 2);
    EXPECT_EQ(A("MID$"), 2); // minimum arity
    // Special cases
    EXPECT_EQ(A("INSTR"), 2); // minimum arity
    EXPECT_EQ(A("SCREEN"), 2); // minimum arity
    // Default unknowns -> 1
    EXPECT_EQ(A("FOO"), 1);
}

