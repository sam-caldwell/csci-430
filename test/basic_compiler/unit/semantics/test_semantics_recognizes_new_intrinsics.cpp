// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathExt.RecognizesNewIntrinsics
 * Purpose: Validate that semantics recognizes extended intrinsics RND, CINT, CSNG, and CDBL.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic recognition)
 * Expected Behavior: Compilation succeeds and produces non-empty IR for code using these intrinsics.
 */
/*
Test: SemanticsMathExt.RecognizesNewIntrinsics
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsMathExt, RecognizesNewIntrinsics) {
    const auto src =
        "10 PRINT RND(1)\n"
        "20 PRINT CINT(2.4)\n"
        "30 PRINT CSNG(3.14)\n"
        "40 PRINT CDBL(5)\n"
        "50 END\n";
    const const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
