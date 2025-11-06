// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenIfBlock.MoreTransfers_And_Calls
 * Purpose: Hit remaining emitIfBlock branches: GOSUB inline in THEN/ELSE, SYSTEM in THEN,
 *          and RANDOMIZE with explicit seed in ELSE.
 */
TEST(CodeGenIfBlock, MoreTransfers_And_Calls) {
    const auto src =
        "10 IF 1=1 THEN\n"
        "20 GOSUB 600\n"
        "21 SYSTEM\n"
        "22 PRINT \"T\"\n"
        "30 ELSE\n"
        "40 RANDOMIZE 7\n"
        "41 GOSUB 700\n"
        "42 PRINT 1\n"
        "50 END IF\n"
        "600 RETURN\n"
        "700 RETURN\n"
        "800 END\n";
    std::string ir = Compiler::compileString(src);
    // Look for gosub scaffolding and randomize
    ASSERT_NE(ir.find("_gosub_entry"), std::string::npos);
    ASSERT_NE(ir.find("_gosub_cont"), std::string::npos);
    ASSERT_NE(ir.find("@srand48"), std::string::npos);
}

