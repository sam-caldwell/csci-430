// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: CodeGen.NumericPromotionAndStoreCasts
Inputs: BASIC with mixed-typed vars: A%, B&, C!, D#, and a sum into E#
Code under test: LLVM IR codegen for loads/stores and casts
Expected behavior: Loads promote to double (sitofp/fpext); stores cast back (fptosi/fptrunc/store)
*/
TEST(CodeGen, NumericPromotionAndStoreCasts) {
    const auto src =
        "10 LET A% = 1.9\n"
        "20 LET B& = 2.9\n"
        "30 LET C! = 3.25\n"
        "40 LET D# = 4.5\n"
        "50 LET E# = A% + B& + C! + D#\n"
        "60 END\n";

    std::string ir = Compiler::compileString(src);

    // Allocations for scalar variables by kind
    ASSERT_NE(ir.find("alloca i16"), std::string::npos) << "expected i16 alloca for %";
    ASSERT_NE(ir.find("alloca i32"), std::string::npos) << "expected i32 alloca for &";
    ASSERT_NE(ir.find("alloca float"), std::string::npos) << "expected float alloca for !";
    ASSERT_NE(ir.find("alloca double"), std::string::npos) << "expected double alloca for #";

    // Stores cast from double to target type
    ASSERT_NE(ir.find("fptosi double 1.9 to i16"), std::string::npos);
    ASSERT_NE(ir.find("fptosi double 2.9 to i32"), std::string::npos);
    ASSERT_NE(ir.find("fptrunc double 3.25 to float"), std::string::npos);
    // Double stores are direct
    ASSERT_NE(ir.find("store double 4.5, ptr"), std::string::npos);

    // Promotions to double when reading for expression math
    ASSERT_NE(ir.find("sitofp i16"), std::string::npos) << "A% should promote via sitofp i16 -> double";
    ASSERT_NE(ir.find("sitofp i32"), std::string::npos) << "B& should promote via sitofp i32 -> double";
    ASSERT_NE(ir.find("fpext float"), std::string::npos) << "C! should extend via fpext float -> double";
}

