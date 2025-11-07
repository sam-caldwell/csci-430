// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include "source_root.h"
#include "run_command.h"

using namespace e2e_helpers;

static std::string cliPath() { return e2e_helpers::sourceRoot() + "/build/basic_compiler/basic_compiler"; }

TEST(CLI, HelpShowsUsage) {
    namespace fs = std::filesystem;
    const std::string cmd = '"' + cliPath() + '"' + " --help 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("Usage:"), std::string::npos);
    ASSERT_NE(out.find("EXIT:0"), std::string::npos);
}

TEST(CLI, NoArgsPrintsUsageAndExits2) {
    const std::string cmd = '"' + cliPath() + '"' + " 2>&1; echo EXIT:$?";
    const std::string out = runCommand(cmd);
    ASSERT_NE(out.find("Usage:"), std::string::npos);
    ASSERT_NE(out.find("EXIT:2"), std::string::npos);
}
