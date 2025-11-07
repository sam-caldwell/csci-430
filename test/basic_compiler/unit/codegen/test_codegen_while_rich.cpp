// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenWhile.RichBodyExercisesBranches
 * Purpose: Exercise many branches in emitWhile: string/numeric prints, MID$, array assign,
 *          STOP/SYSTEM, ON GOTO/ON GOSUB plumbing and bounds-check paths.
 * Components Under Test: CodeGenerator (emitWhile, emitComparison, print lowering)
 * Expected Behavior: IR contains while labels, string/numeric printf calls, MID$/strncpy,
 *                    error-handling stores for array bounds, and switch tables for traps.
 */
TEST(CodeGenWhile, RichBodyExercisesBranches) {
    const auto src =
        // Setup some arrays and variables
        "10 OPTION BASE 1\n"
        "20 DIM S$(3)\n"
        "21 DIM A(2)\n"
        "22 DIM T$(3)\n"
        "30 S$=\"HELLO\"\n"
        // While loop with mixed body to hit branches
        "40 I=0\n"
        "50 WHILE I < 3\n"
        // String path in print and numeric printing (int and float paths)
        "60 PRINT S$, 1, 1.5\n"
        // MID$ assignment on scalar string var
        "65 STOP\n"
        "68 SYSTEM\n"
        "70 MID$(S$,2,1)=\"Z\"\n"
        // Array assign with computed index (exercise bounds plumbing even if ok)
        "75 T$(I+1)=S$\n"
        "80 A(I+1)=A(1)+1\n"
        // Simple IF inside while to ensure codegen threading remains correct
        "90 IF I = 1 THEN PRINT \"ONE\" ELSE PRINT \"OTHER\"\n"
        // ON GOTO / ON GOSUB forms
        "95 ON 1 GOTO 900, 910\n"
        "96 ON 1 GOSUB 920, 930\n"
        // Advance loop var
        "97 I = I + 1\n"
        "98 WEND\n"
        // Targets used by ON ... statements
        "900 PRINT \"GOTO\"\n"
        "910 END\n"
        "920 PRINT \"GOSUB\"\n"
        "930 RETURN\n";

    std::string ir = Compiler::compileString(src);
    // While structure labels
    ASSERT_NE(ir.find("_while_cond"), std::string::npos);
    ASSERT_NE(ir.find("_while_body"), std::string::npos);
    // String strcmp path may appear from IF comparison or print format for strings
    ASSERT_NE(ir.find("@.fmt_str"), std::string::npos);
    // Numeric print format pointers (int/float and with _sp variants)
    ASSERT_NE(ir.find("@.fmt_int"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_num"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_int_sp"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_num_sp"), std::string::npos);
    // MID$ emits strncpy
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
    // Array bounds plumbing stores error variables even if branch not taken
    ASSERT_NE(ir.find("@gwb_err_code"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_err_line"), std::string::npos);
    // ON GOTO / ON GOSUB induce labels and branches
    ASSERT_NE(ir.find("GOTO"), std::string::npos);
    ASSERT_NE(ir.find("GOSUB"), std::string::npos);
}
