// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Semantics.GotoValidTargetCompiles
 * Purpose: Validate that GOTO to an existing line compiles successfully.
 * Components Under Test: Compiler::compileString; Semantics (control-flow resolution)
 * Expected Behavior: Compilation succeeds; IR contains the source label for line10.
 */
/*
Test: Semantics.GotoValidTargetCompiles
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(Semantics, GotoValidTargetCompiles) {
    const auto src =
        "10 GOTO 30\n"
        "20 PRINT 1\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("line10:"), std::string::npos);
}
