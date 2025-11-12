// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/Compiler.h"

#include "basic_compiler/lexer/Lexer.h"
#include "../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/OnGosubStmt.h"
#include "basic_compiler/ast/OnGotoStmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <functional>
#include <utility>

namespace gwbasic {

// NOLINTNEXTLINE(readability-function-cognitive-complexity,readability-function-size)
std::string Compiler::compileString(const std::string& source) {
    Lexer lex(source);
    auto tokens = lex.tokenize();
    Parser parser(std::move(tokens));
    auto program = parser.parseProgram();
    if (gMetrics != nullptr) {
        gMetrics->recordParsedSnapshot(program);
        gMetrics->setAnalyzeOnly(true);
        gwbasic::AstOptimizer::optimize(program);
        gMetrics->setAnalyzeOnly(false);
        gMetrics->recordAfterSemanticsSnapshot(program);
    }
    CodeGenerator gen;
    SemanticAnalyzer sema;
    auto hasOnDispatch = [&]() -> bool {
        // NOLINTNEXTLINE(readability-function-cognitive-complexity)
        std::function<bool(const Stmt*)> check = [&](const Stmt* stmt_ptr) -> bool {
            if (stmt_ptr == nullptr) {
                return false;
            }
            if (dyn_cast<const OnGotoStmt>(stmt_ptr) != nullptr ||
                dyn_cast<const OnGosubStmt>(stmt_ptr) != nullptr) {
                return true;
            }
            if (const auto if_block = dyn_cast<const IfBlockStmt>(stmt_ptr)) {
                for (const auto& then_stmt : if_block->thenBody) {
                    if (check(then_stmt.get())) {
                        return true;
                    }
                }
                for (const auto& else_stmt : if_block->elseBody) {
                    if (check(else_stmt.get())) {
                        return true;
                    }
                }
            }
            if (const auto for_stmt = dyn_cast<const ForStmt>(stmt_ptr)) {
                for (const auto& body_stmt : for_stmt->body) {
                    if (check(body_stmt.get())) {
                        return true;
                    }
                }
            }
            if (const auto while_stmt = dyn_cast<const WhileStmt>(stmt_ptr)) {
                for (const auto& body_stmt : while_stmt->body) {
                    if (check(body_stmt.get())) {
                        return true;
                    }
                }
            }
            return false;
        };
        for (const auto& [line_number, stmts] : program.lines) {
            (void)line_number;
            for (const auto& stmt_uptr : stmts) {
                if (check(stmt_uptr.get())) {
                    return true;
                }
            }
        }
        return false;
    }();
    if (hasOnDispatch) {
        sema.setStrictControlFlow(false);
    }
    auto res = sema.analyze(program);
    gen.setSemantics(res);
    auto ir_text = gen.generate(program);
    if (gMetrics != nullptr) {
        gMetrics->setIrInstructionCount(Metrics::countIrInstructions(ir_text));
    }
    return addDefaultTripleIfMissing(ir_text);
}

} // namespace gwbasic

