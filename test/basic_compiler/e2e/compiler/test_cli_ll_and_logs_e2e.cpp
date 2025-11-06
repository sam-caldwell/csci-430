// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "source_root.h"
#include "run_command.h"

using namespace e2e_helpers;

static std::string cliPath3() { return std::string("basic_compiler/basic_compiler"); }

TEST(CLI, WritesLLFileWhenRequested) {
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_ll";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "a.bas";
    const fs::path ll  = tmp / "a.ll";
    { std::ofstream f(bas); f << "10 PRINT 5\n"; }
    const std::string cmd = '"' + cliPath3() + '"' + " \"" + bas.string() + "\" --ll \"" + ll.string() + "\" --no-logs 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos);
    ASSERT_TRUE(fs::exists(ll));
    // Verify IR content
    std::ifstream in(ll); std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    ASSERT_NE(s.find("define i32 @main()"), std::string::npos);
}

TEST(CLI, PhaseLogsCreatedWhenProvided) {
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_logs";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "b.bas";
    const fs::path ll  = tmp / "b.ll";
    const fs::path lex = tmp / "lex.log";
    const fs::path syn = tmp / "syntax.log";
    const fs::path sem = tmp / "semantic.log";
    const fs::path cg  = tmp / "codegen.log";
    { std::ofstream f(bas); f << "10 LET A=1\n20 PRINT A\n"; }
    std::ostringstream cmd;
    cmd << '"' << cliPath3() << '"'
        << " \"" << bas.string() << "\" --ll \"" << ll.string() << "\""
        << " --lex-log \"" << lex.string() << "\" --syntax-log \"" << syn.string() << "\""
        << " --semantic-log \"" << sem.string() << "\" --log \"" << cg.string() << "\""
        << " 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd.str());
    ASSERT_NE(out.find("EXIT:0"), std::string::npos);
    ASSERT_TRUE(fs::exists(lex));
    ASSERT_TRUE(fs::exists(syn));
    ASSERT_TRUE(fs::exists(sem));
    ASSERT_TRUE(fs::exists(cg));
}

TEST(CLI, StdoutIRWhenNoOutputsRequested) {
    namespace fs = std::filesystem;
    const fs::path tmp = fs::path("..") / "tmp" / "cli_stdout";
    fs::create_directories(tmp);
    const fs::path bas = tmp / "c.bas";
    { std::ofstream f(bas); f << "10 PRINT 5\n"; }
    const std::string cmd = '"' + cliPath3() + '"' + " \"" + bas.string() + "\" --no-logs 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("define i32 @main()"), std::string::npos);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos);
}
