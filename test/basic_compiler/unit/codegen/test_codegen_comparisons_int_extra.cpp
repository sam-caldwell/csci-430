// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: CodeGenComparisons.Int_Le_Gt_Predicates
Inputs: IF A$<=B$ THEN 20 : IF A$>B$ THEN 30
Code under test: emitComparison string compare predicate selection
Expected behavior: IR uses signed integer predicates (icmp sle/sgt) on strcmp result and calls strcmp.
*/
TEST(CodeGenComparisons, Int_Le_Gt_Predicates) {
    const std::string src =
        "10 IF A$<=B$ THEN 20\n"
        "20 IF A$>B$ THEN 30\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    // strcmp call and signed integer compare predicate on its result
    EXPECT_NE(ir.find("@strcmp"), std::string::npos);
    EXPECT_NE(ir.find("icmp sle"), std::string::npos);
}
