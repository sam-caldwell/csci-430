// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/compiler/FileOpenError.h"
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/Compiler.h"

using gwbasic::FileOpenError;
using gwbasic::Compiler;
namespace ph = gwbasic::phase_log_helpers;

TEST(CompilerExceptions, FileOpenError_MessageContainsPath) {
    const std::string p = "/tmp/definitely_not_here.bas";
    FileOpenError ex(p);
    std::string msg = ex.what();
    ASSERT_NE(msg.find(p), std::string::npos);
}

TEST(CompilerExceptions, ParseFileNoLogs_ThrowsFileOpenError) {
    const std::string missing = "/tmp/parse_no_logs_missing.bas";
    EXPECT_THROW({ (void)ph::parseFileNoLogs(missing); }, FileOpenError);
}

TEST(CompilerExceptions, TokenizeRootWithLogs_ThrowsFileOpenError) {
    const std::string missing = "/tmp/tokenize_root_missing.bas";
    std::string canon; int minLine = 0;
    EXPECT_THROW({ (void)ph::tokenizeRootWithLogs(missing, "/tmp/lex.log", "/tmp/syn.log", canon, minLine); }, FileOpenError);
}

TEST(CompilerExceptions, CompileFile_ThrowsFileOpenError) {
    const std::string missing = "/tmp/compile_file_missing.bas";
    EXPECT_THROW({ (void)Compiler::compileFile(missing); }, FileOpenError);
}

TEST(CompilerExceptions, CompileFileWithLog_ThrowsFileOpenError) {
    const std::string missing = "/tmp/compile_file_with_log_missing.bas";
    EXPECT_THROW({ (void)Compiler::compileFileWithLog(missing, "/tmp/codegen.log"); }, FileOpenError);
}

