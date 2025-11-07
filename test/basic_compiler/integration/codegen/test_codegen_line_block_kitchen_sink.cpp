// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.LineBlock_KitchenSink_IR
 * Purpose: Exercise many emit_line_block branches in one program: OPTIONs, OPEN/CLOSE, PRINT USING and zones,
 *          DATA/READ/RESTORE, SWAP, ON GOTO, LINE INPUT. Ensures corresponding IR patterns emit.
 * Components Under Test: CodeGenerator::emitLineBlock
 * Expected Behavior: IR includes fopen/fclose/fprintf paths, snprintf zone padding, data tables, read index reset,
 *                    switch for ON GOTO, and line input scanf.
 */
TEST(Integration, LineBlock_KitchenSink_IR) {
    const char* src =
        "10 OPTION BASE 1\n"
        "20 OPTION PRINTZONES ON\n"
        "30 OPEN \"foo\" FOR OUTPUT AS #1\n"
        "40 PRINT #1, USING \"V:%d\"; 42\n"
        "50 PRINT 1,2;3\n"
        "60 DATA \"X\",123\n"
        "70 READ A$, B\n"
        "80 RESTORE\n"
        "90 READ A$, B\n"
        "100 SWAP B, B\n"
        "110 ON 1 GOTO 200,300\n"
        "120 LINE INPUT A$\n"
        "130 CLOSE #1\n"
        "140 END\n"
        "200 PRINT \"T1\"\n"
        "210 END\n"
        "300 PRINT \"T2\"\n"
        "310 END\n";
    std::string ir = Compiler::compileString(src);
    // File operations
    EXPECT_NE(ir.find("@fopen"), std::string::npos);
    EXPECT_NE(ir.find("@fclose"), std::string::npos);
    EXPECT_NE(ir.find("@fprintf"), std::string::npos);
    // Zone padding uses snprintf and gwb_screen_write
    EXPECT_NE(ir.find("@snprintf"), std::string::npos);
    EXPECT_NE(ir.find("gwb_screen_write"), std::string::npos);
    // Data/Read constructs
    EXPECT_NE(ir.find("@gwb_data"), std::string::npos);
    EXPECT_NE(ir.find("@gwb_data_idx"), std::string::npos);
    // ON GOTO switch
    EXPECT_NE(ir.find("switch i32"), std::string::npos);
    // LINE INPUT uses fgets/scanf
    EXPECT_NE(ir.find("@scanf"), std::string::npos);
}

