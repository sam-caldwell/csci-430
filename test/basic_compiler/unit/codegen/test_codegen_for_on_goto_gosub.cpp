// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: CodeGenFor.OnGotoGosub_InsideBody
Purpose: Exercise emitFor handling of ON GOTO and ON GOSUB statements within
         the loop body, including subroutine inlining. Keeps loop small to
         avoid runtime noise.
Expected IR markers: _on_cont_, _on_gs_entry_*, _on_gs_cont_, and gosub labels.
*/
TEST(CodeGenFor, OnGotoGosub_InsideBody) {
    const char* src =
        "10 FOR I=1 TO 1\n"
        "20 ON 1 GOTO 100,200\n"
        "30 ON 2 GOSUB 300,400\n"
        "40 NEXT I\n"
        "90 END\n"
        "100 END\n"
        "200 END\n"
        "300 RETURN\n"
        "400 RETURN\n";

    std::string ir = Compiler::compileString(src);
    // ON GOTO continuation label
    ASSERT_NE(ir.find("_on_cont_"), std::string::npos);
    // ON GOSUB entries and continuation
    ASSERT_NE(ir.find("_on_gs_entry_"), std::string::npos);
    ASSERT_NE(ir.find("_on_gs_cont_"), std::string::npos);
    // Inline gosub labels should appear
    ASSERT_NE(ir.find("_gosub_entry"), std::string::npos);
}

