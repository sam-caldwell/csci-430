// (c) 2025 Sam Caldwell. All Rights Reserved.
#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGen, MkdirRmdirKillName_IR) {
    const std::string src = R"(10 MKDIR "dir1"
20 RMDIR "dir1"
30 KILL "file.txt"
40 NAME "old.txt" AS "new.txt"
)";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@mkdir"), std::string::npos);
    EXPECT_NE(ir.find("@rmdir"), std::string::npos);
    EXPECT_NE(ir.find("@remove"), std::string::npos);
    EXPECT_NE(ir.find("@rename"), std::string::npos);
    // Error-dispatch paths should be present
    EXPECT_NE(ir.find("@gwb_err_code"), std::string::npos);
}

TEST(CodeGen, Shell_Beep_Environ_IR) {
    const std::string src = R"(10 SHELL "echo hi"
20 BEEP
30 ENVIRON "FOO=BAR"
)";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@system"), std::string::npos);
    EXPECT_NE(ir.find(".bell"), std::string::npos);
    EXPECT_NE(ir.find("@printf"), std::string::npos);
    EXPECT_NE(ir.find("@strstr"), std::string::npos);
    EXPECT_NE(ir.find("@setenv"), std::string::npos);
}

