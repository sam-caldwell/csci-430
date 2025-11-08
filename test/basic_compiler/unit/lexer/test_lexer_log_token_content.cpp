// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/Lexer.h"

using namespace gwbasic;

/***
Test: Lexer.LogToken_FormatsTokensAndEscapes
Purpose: Ensure logToken writes expected format for various token types and
         escapes lexemes securely (quotes, backslashes, control bytes).
Security: Validates NUL/control bytes render as \xHH and quotes/backslashes are escaped.
*/
TEST(Lexer, LogToken_FormatsTokensAndEscapes) {
    namespace fs = std::filesystem;
    fs::path tmpdir = fs::path("..") / "tmp" / "lexer_log_token_content";
    fs::create_directories(tmpdir);
    fs::path logpath = tmpdir / "lex_token_content.log";
    std::error_code ec; fs::remove(logpath, ec);

    // Build a string literal payload with control bytes, embedded quote, and backslash
    std::string payload;
    payload.push_back('A');
    payload.push_back('\0');       // NUL
    payload.push_back('\x01');     // SOH
    payload.push_back('"');        // double quote (will be doubled in source)
    payload.push_back('\\');       // backslash
    payload.push_back('\x7F');     // DEL

    // Double quotes for BASIC source so that the embedded '"' appears in the token lexeme
    std::string quoted = "\""; // starting quote
    for (unsigned char ch : payload) {
        if (ch == '"') quoted += "\"\""; else quoted.push_back(static_cast<char>(ch));
    }
    quoted += "\""; // ending quote

    // Include an identifier on a second line to verify IDENT logging as well
    const std::string src = std::string("10 PRINT ") + quoted + "\n20 Foo=1\n";
    {
        Lexer lex(src);
        lex.setLexLogPath(logpath.string());
        auto toks = lex.tokenize();
        ASSERT_FALSE(toks.empty());
        // Leave scope to flush/close the log file
    }

    std::ifstream in(logpath);
    ASSERT_TRUE(in.good());
    std::string log((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    // Expect an INT token (line number)
    EXPECT_NE(log.find("token INT @ "), std::string::npos);
    // Expect a String token line that includes escaped lexeme
    // Specifically: A, then \x00 (NUL), then \x01, then \" for quote, \\\\ for backslash, and \x7F for DEL
    EXPECT_NE(log.find("token STRING @ "), std::string::npos);
    EXPECT_NE(log.find("A\\x00\\x01\\\"\\\\\\x7F"), std::string::npos);
    // Expect an IDENT token for Foo and that its lexeme appears quoted
    EXPECT_NE(log.find("token IDENT @ "), std::string::npos);
    EXPECT_NE(log.find("\"Foo\""), std::string::npos);

    // NEWLINE tokens present in log
    EXPECT_NE(log.find("token NEWLINE @ "), std::string::npos);

    // EndOfFile likewise should not include a quoted lexeme
    // EOF token present in log
    EXPECT_NE(log.find("token EOF @ "), std::string::npos);
}

/***
Test: Lexer.LogToken_BadPathOrDisabled_NoCrash
Purpose: Exercise sad paths: disabled logger by default (no path set) and
         invalid path (directory) via setLexLogPath; ensure no crashes.
*/
TEST(Lexer, LogToken_BadPathOrDisabled_NoCrash) {
    // Disabled (no path set): should not crash during tokenize
    {
        Lexer lx("10 PRINT \"X\"\n");
        EXPECT_NO_THROW({ (void)lx.tokenize(); });
    }
    // Invalid path (directory): logger open fails; logging goes to null sink
    namespace fs = std::filesystem;
    fs::path dir = fs::path("..") / "tmp" / "lexer_log_token" / "as_dir";
    fs::create_directories(dir);
    {
        Lexer lx("10 PRINT \"Y\"\n");
        lx.setLexLogPath(dir.string());
        EXPECT_NO_THROW({ (void)lx.tokenize(); });
        // Ensure we did not turn a directory into a file
        EXPECT_TRUE(fs::is_directory(dir));
    }
}
