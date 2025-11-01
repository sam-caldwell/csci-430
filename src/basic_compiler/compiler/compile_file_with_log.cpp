// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include <fstream>
#include <sstream>

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
std::string Compiler::compileFileWithLog(const std::string& path, const std::string& logPath) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error(std::string("Unable to open input file: ").append(path));
    Lexer lex(in);
    auto tokens = lex.tokenize();
    Parser parser(std::move(tokens));
    auto program = parser.parseProgram();
    CodeGenerator gen;
    gen.setLogPath(logPath);
    // Keep semantics integration consistent
    SemanticAnalyzer sema;
    auto res = sema.analyze(program);
    gen.setSemantics(res);
    return gen.generate(program);
}

} // namespace gwbasic
