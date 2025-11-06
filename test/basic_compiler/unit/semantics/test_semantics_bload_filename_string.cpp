// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/*
 * Test: SemanticsMemIO.BloadFilenameString
 * Inputs: BLOAD 123 (non-string filename)
 * Code under test: Parser/Compiler for BLOAD argument types.
 * Expected behavior: ParseError thrown because filename must be string.
 */
TEST(SemanticsMemIO, BloadFilenameString) {
    const char* src =
        "10 BLOAD 123\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, ParseError);
}
