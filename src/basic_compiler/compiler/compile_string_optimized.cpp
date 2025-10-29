// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include "basic_compiler/opt/AstOptimizer.h"

namespace gwbasic {

std::string Compiler::compileStringOptimized(const std::string& source) {
    Lexer lex(source);
    auto tokens = lex.tokenize();
    Parser parser(std::move(tokens));
    auto program = parser.parseProgram();
    AstOptimizer opt;
    opt.optimize(program);
    CodeGenerator gen;
    return gen.generate(program);
}

} // namespace gwbasic

