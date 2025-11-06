// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: Integration.RESTORE_ResetsDataIdxIR
Inputs: Program with RESTORE
Expected: IR contains 'store i32 0, ptr @gwb_data_idx'
*/
TEST(Integration, RESTORE_ResetsDataIdxIR) {
    std::string src = R"(10 RESTORE
20 END
)";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("store i32 0, ptr @gwb_data_idx"), std::string::npos);
}

