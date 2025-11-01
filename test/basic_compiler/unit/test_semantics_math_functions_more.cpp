// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

TEST(SemanticsMathMore, UnknownFunctionErrors) {
    const auto src =
        "10 PRINT FOO(1)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsMathMore, ArityErrorsReported) {
    // recognized function but wrong arity
    const auto src =
        "10 PRINT TAN(1,2)\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsMathMore, TypeErrorsOnStringArgs) {
    const auto src =
        "10 PRINT INT(\"A\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsMathMore, SqrtAliasSQRTRecognized) {
    const auto src =
        "10 PRINT SQRT(16)\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("call double @sqrt(double 16.0)"), std::string::npos);
}

