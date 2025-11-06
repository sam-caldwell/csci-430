// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/Compiler.h"

namespace gwbasic {

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
std::string Compiler::compileStringWithPhaseLogs(const std::string& source,
                                                 const std::string& lexLogPath,
                                                 const std::string& syntaxLogPath,
                                                 const std::string& semanticLogPath,
                                                 const std::string& codegenLogPath) {
    Lexer lex(source);
    lex.setLexLogPath(lexLogPath);
    auto tokens = lex.tokenize();
    Parser parser(std::move(tokens));
    parser.setSyntaxLogPath(syntaxLogPath);
    auto program = parser.parseProgram();
    if (gMetrics) {
        Metrics::computeAstSnapshot(program, gMetrics->ast_parsed);
        gMetrics->analyze_only = true;
        gwbasic::AstOptimizer::optimize(program);
        gMetrics->analyze_only = false;
        Metrics::computeAstSnapshot(program, gMetrics->ast_after_semantics);
    }
    // Semantic analysis (scope + references + strings)
    SemanticAnalyzer sema;
    sema.setLogPath(semanticLogPath);
    const auto semRes = sema.analyze(program);
    CodeGenerator gen;
    if (!codegenLogPath.empty()) gen.setLogPath(codegenLogPath);
    gen.setSemantics(semRes);
    auto ir = gen.generate(program);
    if (gMetrics) gMetrics->codegen.ir_instructions = countIrInstructions(ir);
    return Compiler::addDefaultTripleIfMissing(ir);
}

} // namespace gwbasic
