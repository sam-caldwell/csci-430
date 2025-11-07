// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/ir_match.h"

using namespace gwbasic;

/***
 * Test: CodeGenPrint.ChannelAndUsingIR
 * Purpose: Validate PRINT #n, USING ... routes to fprintf and PRINT USING routes to printf with format override.
 * Components Under Test: CodeGenerator PRINT channel and USING handling, file table lookup.
 * Expected Behavior: IR includes @fprintf(ptr ...) with @gwb_files usage and @printf(ptr ...) for stdout case.
 */
TEST(CodeGenPrint, ChannelAndUsingIR) {
    const auto src =
        "10 PRINT #2, USING \"%d %0.2f\", 5, 3.14\n"
        "20 PRINT USING \"%f\", 7/2\n";
    const std::string ir = Compiler::compileString(src);
    // Expect fprintf path for channel (#2)
    EXPECT_TRUE(irtest::irContainsAny(ir, {"@fprintf(ptr"}));
    EXPECT_TRUE(ir.find("@gwb_files") != std::string::npos);
    // Expect printf path for stdout with USING override present
    EXPECT_TRUE(irtest::irContainsAny(ir, {"@printf(ptr"}));
}
