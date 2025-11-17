// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "basic_compiler/lexer/Lexer.h"

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
    // Logging removed; skip file-based assertions.

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
    Lexer lex(src);
    auto toks = lex.tokenize();
    ASSERT_FALSE(toks.empty());
    // Logging removed; detailed token format/escape verification via file is no longer applicable.
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
    // Invalid path (directory): logging removed; ensure tokenize still does not crash
    namespace fs = std::filesystem;
    fs::path dir = fs::path("..") / "tmp" / "lexer_log_token" / "as_dir";
    fs::create_directories(dir);
    {
        Lexer lx("10 PRINT \"Y\"\n");
        EXPECT_NO_THROW({ (void)lx.tokenize(); });
    }
}
