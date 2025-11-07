// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: Integration.CHDIR_IRContainsChdir
 * Purpose: Ensure CHDIR emits chdir() call in IR.
 */
TEST(Integration, CHDIR_IRContainsChdir) {
    const char* src =
        "10 CHDIR \"/tmp\"\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    ASSERT_NE(ir.find("@chdir"), std::string::npos);
}

