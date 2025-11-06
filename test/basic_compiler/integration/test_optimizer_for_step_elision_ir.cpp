// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: OptimizerFor.StepElision
Inputs: A FOR loop with STEP 1
Code under test: AstOptimizer::optimize (For step elision) + CodeGenerator::emitFor
Expected behavior: STEP 1 is elided; IR uses literal 1.0 in step compare (fcmp olt double 1.0, 0.0)
*/
TEST(OptimizerFor, StepElision) {
    const char* src =
        "10 FOR I=1 TO 3 STEP 1: PRINT I: NEXT I\n"
        "20 END\n";
    const std::string ir = Compiler::compileStringOptimized(src);
    // Condition for step sign check uses 1.0 literal when step is elided
    EXPECT_NE(ir.find(" = fcmp olt double 1.0, 0.0"), std::string::npos);
}

