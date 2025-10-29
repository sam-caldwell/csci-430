// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include <fstream>
#include <sstream>

namespace gwbasic {

std::string Compiler::compileStringWithPhaseLogs(const std::string& source,
                                                 const std::string& lexLogPath,
                                                 const std::string& syntaxLogPath,
                                                 const std::string& semanticLogPath,
                                                 const std::string& codegenLogPath) {
    /*
     * Function: Compiler::compileStringWithPhaseLogs
     * Inputs:
     *  - source: Complete GW-BASIC source as a single string
     *  - lexLogPath: File to append lexical events (tokens with positions)
     *  - syntaxLogPath: File to append syntax parse events (node, line/col)
     *  - semanticLogPath: File to append semantic events (vars/refs/loops)
     *  - codegenLogPath: File to append IR emission events per AST node
     * Outputs:
     *  - std::string: LLVM IR text for the compiled program
     * Theory of operation:
     *  - Executes the pipeline while enabling detailed logs at the parser and
     *    code generator stages to correlate source to structure and emitted IR.
     */
    Lexer lex(source);
    lex.setLexLogPath(lexLogPath);
    auto tokens = lex.tokenize();
    Parser parser(std::move(tokens));
    parser.setSyntaxLogPath(syntaxLogPath);
    auto program = parser.parseProgram();
    CodeGenerator gen;
    gen.setSemanticLogPath(semanticLogPath);
    if (!codegenLogPath.empty()) gen.setLogPath(codegenLogPath);
    return gen.generate(program);
}

std::string Compiler::compileFileWithPhaseLogs(const std::string& path,
                                               const std::string& lexLogPath,
                                               const std::string& syntaxLogPath,
                                               const std::string& semanticLogPath,
                                               const std::string& codegenLogPath) {
    /*
     * Function: Compiler::compileFileWithPhaseLogs
     * Inputs:
     *  - path: Filesystem path to a GW-BASIC source file
     *  - lexLogPath: Destination for lexical phase log
     *  - syntaxLogPath: Destination for syntax phase log
     *  - semanticLogPath: Destination for semantic phase log
     *  - codegenLogPath: Destination for code generation log
     * Outputs:
     *  - std::string: LLVM IR text for the compiled program
     * Theory of operation:
     *  - Reads file contents and forwards to compileStringWithPhaseLogs() so
     *    string- and file-based flows share identical behavior and logging.
     */
    std::ifstream in(path);
    if (!in) throw std::runtime_error(std::string("Unable to open input file: ").append(path));
    std::ostringstream buf;
    buf << in.rdbuf();
    return compileStringWithPhaseLogs(buf.str(), lexLogPath, syntaxLogPath, semanticLogPath, codegenLogPath);
}

} // namespace gwbasic
