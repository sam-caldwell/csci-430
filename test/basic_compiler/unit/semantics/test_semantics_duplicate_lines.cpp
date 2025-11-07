// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test Suite: Semantics duplicate line numbers
 * Purpose: Ensure duplicates are rejected to avoid invalid IR labels.
 */
/***
Test: Semantics.DuplicateLineNumbersError
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(Semantics, DuplicateLineNumbersError) {
    const auto src =
        "10 PRINT 1\n"
        "10 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
