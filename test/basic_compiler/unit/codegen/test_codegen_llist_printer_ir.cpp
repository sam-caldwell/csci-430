// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenLLIST.EmitsPrinterOrStdoutFallback
 * Purpose: Ensure LLIST references @gwb_files[0] and either writes to printer
 *          (fwrite) or safely falls back to stdout + screen mirror when the
 *          printer channel is not open.
 */
TEST(CodeGenLLIST, EmitsPrinterOrStdoutFallback) {
    const char* src =
        "10 LLIST\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 0"), std::string::npos);
    // Must branch based on null printer handle
    ASSERT_NE(ir.find("icmp eq ptr"), std::string::npos);
    // Either path exists depending on branch: fwrite or screen-write fallback
    bool hasFwrite = (ir.find("@fwrite") != std::string::npos);
    bool hasScreen = (ir.find("@gwb_screen_write") != std::string::npos);
    ASSERT_TRUE(hasFwrite || hasScreen);
}
