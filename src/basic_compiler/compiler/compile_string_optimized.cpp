// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/Compiler.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

#include <string>
#include <utility>

namespace gwbasic {

/*
 * Function: Compiler::compileStringOptimized
 * Inputs:
 *  - source: GW-BASIC program text
 * Outputs:
 *  - std::string: Optimized LLVM IR (.ll) for the program
 * Theory of operation:
 *  - Tokenizes and parses the source, applies AST optimizations, runs
 *    semantics, and generates IR with a default triple header.
 */
std::string Compiler::compileStringOptimized(const std::string& source) {
    Lexer lex(source);
    // debug disabled
    auto tokens = lex.tokenize();
    
    Parser parser(std::move(tokens));
    auto program = parser.parseProgram();
    
    if (gMetrics != nullptr) { gMetrics->recordParsedSnapshot(program); }
    
    gwbasic::AstOptimizer::optimize(program);
    
    if (gMetrics != nullptr) { gMetrics->recordOptimizedSnapshot(program); }
    CodeGenerator gen;
    SemanticAnalyzer sema;
    sema.setStrictControlFlow(false);
    
    auto res = sema.analyze(program);
    gen.setSemantics(res);
    
    auto irText = gen.generate(program);
    
    if (gMetrics != nullptr) { gMetrics->setIrInstructionCount(Metrics::countIrInstructions(irText)); }
    
    return Compiler::addDefaultTripleIfMissing(irText);
}

} // namespace gwbasic
