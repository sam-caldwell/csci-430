// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Semantics.MixedStringNumberComparisonErrors
 * Purpose: Validate that comparisons between string and numeric types are rejected.
 * Components Under Test: Compiler::compileString; Semantics (type checking for comparisons)
 * Expected Behavior: Compilation throws SemanticError for expressions like "\"A\" = 1".
 */
/*
Test: Semantics.MixedStringNumberComparisonErrors
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(Semantics, MixedStringNumberComparisonErrors) {
    const auto src =
        "10 IF \"A\" = 1 THEN 20\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
