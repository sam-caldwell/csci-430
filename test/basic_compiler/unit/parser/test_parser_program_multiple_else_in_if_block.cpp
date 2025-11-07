// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

TEST(Parser, ParseProgram_MultipleElseInIfBlock) {
    std::string src =
        "10 IF 1 THEN\n"
        "20 ELSE\n"
        "30 ELSE\n"
        "40 END IF\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
}

