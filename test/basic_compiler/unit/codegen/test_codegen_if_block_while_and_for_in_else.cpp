// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenIfBlock.WhileAndForInThenElse
 * Purpose: Ensure emitIfBlock delegates to emitWhile/emitFor for constructs
 *          appearing in THEN and ELSE bodies, respectively.
 */
TEST(CodeGenIfBlock, WhileAndForInThenElse) {
    const char* src =
        "10 I=0: J=0\n"
        "20 IF 1<2 THEN\n"
        // THEN: simple WHILE loop
        "30 WHILE I<1: PRINT 1: I=I+1: WEND\n"
        "40 ELSE\n"
        // ELSE: simple FOR loop
        "50 FOR J=1 TO 1: PRINT 2: NEXT J\n"
        // ELSE: and a WHILE as well
        "60 WHILE J<1: PRINT 3: WEND\n"
        "70 END IF\n"
        "80 END\n";

    const std::string ir = Compiler::compileString(src);
    // IF labels present
    ASSERT_NE(ir.find("_if_then"), std::string::npos);
    ASSERT_NE(ir.find("_if_else"), std::string::npos);
    // FOR/WHILE labels should appear
    ASSERT_NE(ir.find("_for_body"), std::string::npos);
    ASSERT_NE(ir.find("_while_cond"), std::string::npos);
}
