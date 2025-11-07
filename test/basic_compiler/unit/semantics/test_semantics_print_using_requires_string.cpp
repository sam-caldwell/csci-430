// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
Test: SemanticsPrintUsing.RequiresStringFormat
Inputs: PRINT USING("%d"), 7 (valid) and PRINT USING(1), 7 (invalid)
Code under test: SemanticAnalyzer::analyzeStmt for PrintStmt
Expected behavior: Accepts string format; rejects non-string format with SemanticError
*/
TEST(SemanticsPrintUsing, RequiresStringFormat) {
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 PRINT USING(\"%d\"), 7\n"); });
    EXPECT_THROW({ (void)Compiler::compileString("10 PRINT USING(1), 7\n"); }, SemanticError);
}

