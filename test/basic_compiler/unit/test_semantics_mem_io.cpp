// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsMemIO.PokeRequiresNumeric
 * Inputs: POKE with string address argument.
 * Code under test: Compiler::compileString() + semantics for POKE types.
 * Expected behavior: SemanticError thrown due to non-numeric address.
 */
TEST(SemanticsMemIO, PokeRequiresNumeric) {
    const char* src =
        "10 POKE \"A\", 1\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
