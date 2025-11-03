// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

TEST(SemanticsColor, ForegroundMustBeNumeric) {
    const char* src =
        "10 COLOR \"A\"\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsColor, BackgroundMustBeNumeric) {
    const char* src =
        "10 COLOR 1,\"B\"\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsColor, BorderMustBeNumeric) {
    const char* src =
        "10 COLOR 1,2,\"C\"\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

