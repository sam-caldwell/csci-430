// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include <fstream>
#include <sstream>

namespace gwbasic {
/*
 * Function: Compiler::compileFile
 * Inputs:
 *  - path: filesystem path to GW-BASIC source file
 * Outputs:
 *  - std::string: LLVM IR text of the compiled program
 * Theory of operation:
 *  - Reads file contents into memory, delegates to compileString(), which
 *    lexes, parses, and generates IR for the program.
 */
std::string Compiler::compileFile(const std::string& path) {

    std::ifstream in(path);
    if (!in) throw std::runtime_error(std::string("Unable to open input file: ").append(path));
    Lexer lex(in);
    auto tokens = lex.tokenize();
    Parser parser(std::move(tokens));
    auto program = parser.parseProgram();
    // Keep semantics integration consistent with compileString
    SemanticAnalyzer sema;
    auto res = sema.analyze(program);
    CodeGenerator gen;
    gen.setSemantics(res);
    return gen.generate(program);
}

} // namespace gwbasic
