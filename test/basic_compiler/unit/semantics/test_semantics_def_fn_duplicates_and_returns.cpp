// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.DefFn_DuplicateDefinitionErrors
Inputs: Two DEF FN for same name (case-insensitive)
Expected: SemanticError duplicate DEF
*/
TEST(Semantics, DefFn_DuplicateDefinitionErrors) {
    const char* src =
        "10 DEF FNA(X)=X+1\n"
        "20 DEF fna(Y)=Y+2\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

/***
Test: Semantics.DefFn_ReturnTypeMismatchBothWays
Inputs: DEF FN$ returns number; DEF FN returns string
Expected: SemanticError in both
*/
TEST(Semantics, DefFn_ReturnTypeMismatchBothWays) {
    // String-suffixed name returns number
    {
        const char* src = "10 DEF FNS$(X)=X+1\n";
        EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
    }
    // Non-suffixed name returns string
    {
        const char* src = "10 DEF FNT(X)=\"A\"\n";
        EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
    }
}

