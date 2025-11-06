// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/OnErrorGotoStmt.h"
#include "basic_compiler/ast/ResumeStmt.h"
#include "basic_compiler/ast/ErrorStmt.h"

using namespace gwbasic;

TEST(Parser, OnErrorGotoAndResumeParse) {
    std::string src =
        "10 ON ERROR GOTO 100\n"
        "20 ERROR 5\n"
        "30 RESUME NEXT\n"
        "40 RESUME 0\n"
        "50 RESUME 200\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 5u);
    // Check statements kinds per line
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    ASSERT_TRUE(dyn_cast<OnErrorGotoStmt>(prog.lines[0].statements[0].get()));
    {
        auto* n = dyn_cast<OnErrorGotoStmt>(prog.lines[0].statements[0].get());
        ASSERT_EQ(n->targetLine, 100);
    }
    ASSERT_TRUE(dyn_cast<ErrorStmt>(prog.lines[1].statements[0].get()));
    ASSERT_TRUE(dyn_cast<ResumeStmt>(prog.lines[2].statements[0].get()));
    {
        auto* r = dyn_cast<ResumeStmt>(prog.lines[2].statements[0].get());
        ASSERT_EQ(r->kind, ResumeStmt::Kind::Next);
    }
    ASSERT_TRUE(dyn_cast<ResumeStmt>(prog.lines[3].statements[0].get()));
    {
        auto* r = dyn_cast<ResumeStmt>(prog.lines[3].statements[0].get());
        ASSERT_EQ(r->kind, ResumeStmt::Kind::Reexecute);
    }
    ASSERT_TRUE(dyn_cast<ResumeStmt>(prog.lines[4].statements[0].get()));
    {
        auto* r = dyn_cast<ResumeStmt>(prog.lines[4].statements[0].get());
        ASSERT_EQ(r->kind, ResumeStmt::Kind::Line);
        ASSERT_EQ(r->line, 200);
    }
}

TEST(Parser, OnErrorGotoMissingLineErrors) {
    std::string src = "10 ON ERROR GOTO\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}

