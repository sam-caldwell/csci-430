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
#include "../../helper/source_root.h"

using namespace gwbasic;
using namespace e2e_helpers;

/*
 * Test: E2E.GraphicsCircleDemo_PrintsOk
 * Inputs: demos/graphics-circle.bas compiled end-to-end
 * Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
 * Expected behavior: Program compiles and runs; prints confirmation line
 */
TEST(E2E, GraphicsCircleDemo_PrintsOk) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    // Compile demo from file
    std::string ir = Compiler::compileFile((e2e_helpers::sourceRoot()+"/demos/graphics-circle.bas").c_str());
    ASSERT_FALSE(ir.empty());

    // Write IR and compile with clang
    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_gfx_circle_demo";
    std::filesystem::create_directories(tmp);
    const auto ll = tmp / "program.ll";
    const auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    const int ec = std::system(c1.str().c_str());
    ASSERT_EQ(ec, 0);

    // Run and capture output
    const std::string out = runCommand(std::string("\"") + bin.string() + "\"");

    // Validate text line
    ASSERT_NE(out.find("GRAPHICS CIRCLE DEMO OK\n"), std::string::npos);
}

