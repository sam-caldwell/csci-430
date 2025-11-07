// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenDefFn.ParamBinding_DoesNotLoadParamVar
 * Inputs: DEF FNSQ(X)=X*X; DEF FNA$(S$)=S$+"!"; PRINT calls
 * Code under test: CodeGenerator inline DEF FN expansion and parameter binding
 * Expected behavior: IR does not allocate/load parameter variables (uses SSA binding instead).
 */
TEST(CodeGenDefFn, ParamBinding_DoesNotLoadParamVar) {
    const char* src =
        "10 DEF FNSQ(X) = X*X\n"
        "20 PRINT FNSQ(3)\n"
        "30 DEF FNA$(S$) = S$ + \"!\"\n"
        "40 PRINT FNA$(\"ok\")\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    // Ensure parameter variable names are not allocated/loaded (bound via SSA)
    ASSERT_EQ(ir.find("%X"), std::string::npos);
    ASSERT_EQ(ir.find("%S$"), std::string::npos);
    // User function names should not appear as calls in IR
    ASSERT_EQ(ir.find("FNSQ"), std::string::npos);
    ASSERT_EQ(ir.find("FNA$"), std::string::npos);
}

