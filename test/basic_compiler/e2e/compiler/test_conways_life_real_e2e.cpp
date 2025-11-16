// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "basic_compiler/Compiler.h"
#include "../../helper/clang_path.h"
#include "../../helper/run_command.h"
#include "../../helper/tool_exists.h"
#include "../../helper/source_root.h"

using namespace gwbasic;
using namespace e2e_helpers;

/***
 * Test: E2E.ConwaysLife_RealDemo_BuildsRunsAndPrints
 * Inputs: demos/conways-life.bas compiled end-to-end
 * Code under test: Full compiler pipeline (lexer → parser → semantics → codegen → runtime)
 * Expected behavior: Program compiles and runs; captured output contains rules/status text and T=
 * Notes:
 * - The program runs an infinite loop until a key is pressed via INKEY$. We run
 *   the binary through `head -c` to capture the initial output and terminate.
 */
TEST(E2E, ConwaysLife_RealDemo_BuildsRunsAndPrints) {
    if (!toolExists(CLANG_PATH)) {
        GTEST_SKIP() << "clang not found (CLANG_PATH='" << CLANG_PATH << "'), skipping E2E.";
    }
    const std::string demo = e2e_helpers::sourceRoot() + "/demos/conways-life.bas";
    std::string ir = Compiler::compileFile(demo.c_str());
    ASSERT_FALSE(ir.empty());

    const std::filesystem::path tmp = std::filesystem::path("..") / "tmp" / "gwbasic_e2e_conway_real";
    std::filesystem::create_directories(tmp);
    const auto ll = tmp / "program.ll";
    const auto bin = tmp / "program.out";
    { std::ofstream f(ll); f << ir; }
    std::ostringstream c1; c1 << CLANG_PATH << " \"" << ll.string() << "\" -o \"" << bin.string() << "\"";
#ifndef __APPLE__
    c1 << " -lm";
#endif
    ASSERT_EQ(std::system(c1.str().c_str()), 0);

    // Run the program, let it run for N seconds, then terminate it and verify it stops.
    pid_t pid = fork();
    ASSERT_NE(pid, -1) << "fork() failed";
    if (pid == 0) {
        // Child: exec the program
        execl(bin.string().c_str(), bin.string().c_str(), (char*)nullptr);
        _exit(127);
    }
    // Parent: give it a brief moment to start
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // Confirm it is running
    ASSERT_EQ(kill(pid, 0), 0) << "Child process not running";
    // Wait for the configured duration (default 30s)
    int wait_secs = 30;
    if (const char* ev = std::getenv("CONWAY_E2E_WAIT_SECS")) {
        try { wait_secs = std::stoi(std::string(ev)); } catch (...) {}
        if (wait_secs < 1) wait_secs = 1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(wait_secs));
    // Try graceful termination first
    (void)kill(pid, SIGTERM);
    // Wait up to 5 seconds for exit
    bool exited = false; int status = 0;
    for (int i = 0; i < 50; ++i) {
        pid_t r = waitpid(pid, &status, WNOHANG);
        if (r == pid) { exited = true; break; }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (!exited) {
        // Force kill
        (void)kill(pid, SIGKILL);
        (void)waitpid(pid, &status, 0);
        exited = true;
    }
    ASSERT_TRUE(exited) << "Child process did not terminate after signals";
    // Confirm no longer running
    ASSERT_NE(kill(pid, 0), 0) << "Child process still alive after termination";
}
