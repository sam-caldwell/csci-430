// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

TEST(SemanticsMemIO, PokeRequiresNumeric) {
    const char* src =
        "10 POKE \"A\", 1\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}

TEST(SemanticsMemIO, BloadFilenameString) {
    const char* src =
        "10 BLOAD 123\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, ParseError);
}

TEST(SemanticsMemIO, BsaveTypes) {
    const char* src =
        "10 BSAVE \"f\", \"X\", 10\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
