// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/FilesStmt.h"

using namespace gwbasic;

/***
Test: Parser.FILES_Variants
Inputs: 'FILES' (no args), 'FILES "*.BAS"', 'FILES LPT1$,', 'FILES LPT1$, "*.BAS"'
Code under test: Parser::parseFiles
Expected behavior: device/pattern assignment per grammar
*/
TEST(Parser, FILES_Variants) {
    const char* src =
        "10 FILES\n"
        "20 FILES \"*.BAS\"\n"
        "30 FILES LPT1$,\n"
        "40 FILES LPT1$, \"*.BAS\"\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 4u);

    auto* f1 = dynamic_cast<FilesStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(f1, nullptr);
    EXPECT_EQ(f1->device, nullptr);
    EXPECT_EQ(f1->pattern, nullptr);

    auto* f2 = dynamic_cast<FilesStmt*>(prog.lines[1].statements[0].get());
    ASSERT_NE(f2, nullptr);
    EXPECT_EQ(f2->device, nullptr);
    ASSERT_NE(f2->pattern, nullptr);

    auto* f3 = dynamic_cast<FilesStmt*>(prog.lines[2].statements[0].get());
    ASSERT_NE(f3, nullptr);
    ASSERT_NE(f3->device, nullptr);
    EXPECT_EQ(f3->pattern, nullptr);

    auto* f4 = dynamic_cast<FilesStmt*>(prog.lines[3].statements[0].get());
    ASSERT_NE(f4, nullptr);
    ASSERT_NE(f4->device, nullptr);
    ASSERT_NE(f4->pattern, nullptr);
}

