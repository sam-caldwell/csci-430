// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/Compiler.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/compiler/FileOpenError.h"
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

#include <fstream>
#include <string>
#include <utility>

namespace gwbasic {
/*
 * Function: Compiler::compileFileWithLog
 * Inputs:
 *  - path: Filesystem path to a GW-BASIC source file
 *  - logPath: Path for the code generation log output
 * Outputs:
 *  - std::string: LLVM IR text for the compiled program
 * Theory of operation:
 *  - Loads the file contents into memory and delegates to
 *    compileStringWithLog() so the same lex/parse/codegen pipeline and
 *    logging behavior are used for both file and string inputs.
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
std::string Compiler::compileFileWithLog(const std::string& path, const std::string& logPath) {
    std::ifstream inputFile(path);
    if (!inputFile) { throw gwbasic::FileOpenError(path); }
    Lexer lex(inputFile);
    auto tokens = lex.tokenize();
    Parser parser(std::move(tokens));
    parser.setSourcePath(path);
    auto program = parser.parseProgram();
    CodeGenerator gen;
    gen.setLogPath(logPath);
    // Keep semantics integration consistent
    SemanticAnalyzer sema;
    auto res = sema.analyze(program);
    gen.setSemantics(res);
    return Compiler::addDefaultTripleIfMissing(gen.generate(program));
}

} // namespace gwbasic
