// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.POKE_Call_DefUsr_TypeErrors
Inputs: POKE with string address/value; CALL with string; DEF USR with string address
Expected: SemanticError in each case.
*/
TEST(Semantics, POKE_Call_DefUsr_TypeErrors) {
    // POKE address must be numeric
    EXPECT_THROW({ auto ir = Compiler::compileString("10 POKE \"A\", 1\n"); (void)ir; }, SemanticError);
    // POKE value must be numeric
    EXPECT_THROW({ auto ir = Compiler::compileString("10 POKE 1, \"A\"\n"); (void)ir; }, SemanticError);
    // CALL address must be numeric
    EXPECT_THROW({ auto ir = Compiler::compileString("10 CALL \"A\"\n"); (void)ir; }, SemanticError);
    // DEF USR address must be numeric
    EXPECT_THROW({ auto ir = Compiler::compileString("10 DEF USR=\"A\"\n"); (void)ir; }, SemanticError);
}

