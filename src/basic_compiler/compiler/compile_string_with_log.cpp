// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

namespace gwbasic {

std::string Compiler::compileStringWithLog(const std::string& source, const std::string& logPath) {
    /*
     * Function: Compiler::compileStringWithLog
     * Inputs:
     *  - source: Complete GW-BASIC source code as a single string
     *  - logPath: Filesystem path where the code generation log is written
     * Outputs:
     *  - std::string: LLVM IR text for the compiled program
     * Theory of operation:
     *  - Runs the standard pipeline (lex → parse → codegen). The code
     *    generator is configured to emit a detailed log correlating emitted
     *    IR with source lines and AST nodes to the provided logPath.
     */
    Lexer lex(source);
    auto tokens = lex.tokenize();
    Parser parser(std::move(tokens));
    auto program = parser.parseProgram();
    CodeGenerator gen;
    gen.setLogPath(logPath);
    // Provide semantics
    SemanticAnalyzer sema;
    auto res = sema.analyze(program);
    gen.setSemantics(res);
    return gen.generate(program);
}

} // namespace gwbasic
