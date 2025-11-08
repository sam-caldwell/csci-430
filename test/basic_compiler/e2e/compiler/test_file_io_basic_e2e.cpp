// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
Test: E2E.FileOutput_PRINT_ToFile
Inputs: Program opens tmp file for OUTPUT, PRINT #1, "HELLO" and closes
Expected behavior: The file contains a single line "HELLO\n".
*/
TEST(E2E, FileOutput_PRINT_ToFile) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    // Use a simple relative path so the binary writes in the test WORKING_DIRECTORY
    const std::string outfile = "e2e_print_out.txt";
    std::ostringstream src;
    src << "10 OPEN \"" << outfile << "\" FOR OUTPUT AS #1\n"
        << "20 PRINT #1, \"HELLO\"\n"
        << "30 CLOSE #1\n";
    std::string ir = Compiler::compileString(src.str());
    std::filesystem::path ll = "e2e_print.ll"; std::filesystem::path bin = "e2e_print.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    // Run the program (no stdout expected); then check file contents
    (void)runCommand(std::string("./") + bin.string());
    std::ifstream in(outfile);
    ASSERT_TRUE(in.good());
    std::string line; std::getline(in, line);
    EXPECT_EQ(line, "HELLO");
}

/***
Test: E2E.FileInput_LINE_INPUT_FromFile
Inputs: Pre-create input file; program opens FOR INPUT, LINE INPUT #1 into S$, prints it
Expected behavior: Program stdout contains the line without trailing newline from file.
*/
TEST(E2E, FileInput_LINE_INPUT_FromFile) {
    if (!toolExists(CLANG_PATH)) GTEST_SKIP();
    // Pre-create an input file in the test working directory
    const std::string infile = "e2e_input.txt"; { std::ofstream f(infile); f << "Line from file\n"; }
    std::ostringstream src;
    src << "10 OPEN \"" << infile << "\" FOR INPUT AS #1\n"
        << "20 LINE INPUT #1, S$\n"
        << "30 PRINT S$\n";
    std::string ir = Compiler::compileString(src.str());
    std::filesystem::path ll = "e2e_lineinput.ll"; std::filesystem::path bin = "e2e_lineinput.out"; { std::ofstream f(ll); f << ir; }
    std::ostringstream cmd; cmd << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    cmd << " -lm";
#endif
    ASSERT_EQ(std::system(cmd.str().c_str()), 0);
    std::string out = runCommand(std::string("./") + bin.string());
    ASSERT_NE(out.find("Line from file\n"), std::string::npos);
}
