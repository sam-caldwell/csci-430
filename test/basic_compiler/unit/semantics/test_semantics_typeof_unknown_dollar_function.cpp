// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.TypeOf_UnknownDollarFunction_ClassifiesStringThenErrors
Inputs: LET A$ = FOO$()
Code under test: typeOf fallback for names ending with '$' and subsequent analyzeExpr unknown function error
Expected behavior: Compilation throws SemanticError (unknown function), exercising typeOf('$'-suffix) path
*/
TEST(SemanticsTypeOf, UnknownDollarFunction) {
    const char* src =
        "10 LET A$ = FOO$()\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
