// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenFor.Arrays_Mid_Terminators
 * Purpose: Exercise emitFor branches for array assigns (numeric/string), MID$ on array and scalar,
 *          PRINT USING and channel variants, nextStartsWithSpace formatting, and STOP/SYSTEM paths
 *          within the FOR body (terminators placed at end of each loop body).
 */
TEST(CodeGenFor, Arrays_Mid_Terminators) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM A(2,2)\n"
        "21 DIM S$(2,2)\n"
        "25 S$=\"AB\"\n"
        "30 FOR I=1 TO 1\n"
        "40 A(1,I)=I+1\n"
        "45 S$(I,2)=\"X\"\n"
        "47 MID$(S$(1,1),1,1)=\"Q\"\n"
        "50 PRINT USING(\"V=#\"), I\n"
        "55 PRINT #2, \"C\", I\n"
        "58 STOP\n"
        "59 NEXT I\n"
        "60 FOR J=1 TO 1\n"
        "61 MID$(S$,2)=\"R\"\n"
        "62 PRINT J, J+0.75, \" Z\"\n"
        "63 SYSTEM\n"
        "64 NEXT J\n"
        "70 END\n";

    std::string ir = Compiler::compileString(src);
    // Look for array addressing, MID$, formatting, channel printing, and terminators
    ASSERT_NE(ir.find("getelementptr inbounds ["), std::string::npos);
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
    ASSERT_NE(ir.find("@fprintf"), std::string::npos);
    ASSERT_NE(ir.find("@snprintf"), std::string::npos);
    ASSERT_NE(ir.find("_print_int_"), std::string::npos);
    ASSERT_NE(ir.find("_print_flt_"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_num_ns"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_int_ns"), std::string::npos);
    // STOP/SYSTEM appear in IR
    ASSERT_NE(ir.find(".msg_break"), std::string::npos);
}
