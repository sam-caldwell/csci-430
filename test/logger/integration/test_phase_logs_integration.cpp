// File: test/logger/integration/test_phase_logs_integration.cpp
// Purpose: Ensure compiler phase logs generate content via logger integration.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: LoggerIntegration.CompilerPhaseLogsProduceContent
 * Purpose: Ensure Compiler phase logs produce output via Logger integration.
 */
TEST(LoggerIntegration, CompilerPhaseLogsProduceContent) {
  namespace fs = std::filesystem;
  const std::string src =
      "10 LET A = 1\n"
      "20 PRINT A\n"
      "30 END\n";

  const fs::path outdir = fs::current_path() / "logger_integration";
  fs::create_directories(outdir);
  const fs::path lex = outdir / "lex.log";
  const fs::path syn = outdir / "syntax.log";
  const fs::path sem = outdir / "semantic.log";
  const fs::path cg  = outdir / "codegen.log";

  std::string ir = Compiler::compileStringWithPhaseLogs(
      src,
      lex.string(),
      syn.string(),
      sem.string(),
      cg.string()
  );
  EXPECT_NE(ir.find("define i32 @main()"), std::string::npos);

  auto slurp = [](const fs::path& p) {
    std::ifstream in(p); return std::string(
        std::istreambuf_iterator<char>(in),
        std::istreambuf_iterator<char>()
    );
  };
  ASSERT_TRUE(fs::exists(lex));
  ASSERT_TRUE(fs::exists(syn));
  ASSERT_TRUE(fs::exists(sem));
  ASSERT_TRUE(fs::exists(cg));

  EXPECT_NE(slurp(lex).find("token"), std::string::npos);
  EXPECT_NE(slurp(syn).find("line"), std::string::npos);
  EXPECT_NE(slurp(sem).find("VarDecl"), std::string::npos);
  EXPECT_NE(slurp(cg).find("entry ->"), std::string::npos);
}
