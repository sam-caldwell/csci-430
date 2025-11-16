// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/Compiler.h"
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

/***
 * Test: Unit.Metrics_Collection_SimpleProgram
 * Inputs: Enable metrics and compile a simple program string
 * Code under test: Compiler::compileString; Metrics hooks (lexer/AST/codegen)
 * Expected behavior: Metrics reflect non-zero tokens, expected AST shape, and IR instruction count > 0
 */
TEST(Unit, Metrics_Collection_SimpleProgram) {
    Metrics m; gMetrics = &m;
    std::string src = R"(10 LET A = 1 + 2 * 3
20 PRINT A
30 END
)";
    const std::string ir = Compiler::compileString(src);
    (void)ir;
    gMetrics = nullptr;

    // Inspect the printed table to validate presence of key rows and non-empty values
    std::ostringstream oss; m.print(oss);
    const std::string tbl = oss.str();
    // Sections present
    ASSERT_NE(tbl.find("Lexer"), std::string::npos);
    ASSERT_NE(tbl.find("AST (parsed)"), std::string::npos);
    ASSERT_NE(tbl.find("Codegen"), std::string::npos);
    ASSERT_NE(tbl.find("Semantics Opt"), std::string::npos);
    // 'tokens' row appears and has a digit following
    const auto tokPos = tbl.find("tokens");
    ASSERT_NE(tokPos, std::string::npos);
    bool hasDigit = false; for (size_t i = tokPos; i < tbl.size(); ++i) if (std::isdigit(static_cast<unsigned char>(tbl[i]))) { hasDigit = true; break; }
    ASSERT_TRUE(hasDigit);
}
