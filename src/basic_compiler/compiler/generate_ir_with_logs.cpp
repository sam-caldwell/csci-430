// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

#include <string>

namespace gwbasic::phase_log_helpers {

/*
 * Function: generateIRWithLogs
 * Purpose:
 *  - Run semantic analysis and code generation with optional logs enabled.
 * Inputs:
 *  - program: Combined Program AST to compile
 *  - semanticLogPath: Path to write semantic analysis log
 *  - codegenLogPath: Path to write code generation log (if not empty)
 * Outputs:
 *  - std::string: LLVM IR text body (may need target triple prefixing by caller)
 */
std::string generateIRWithLogs(const gwbasic::Program& program,
                               const LogPaths& logs) {
    (void)logs; // logging disabled; retain parameter for API compatibility
    SemanticAnalyzer sema;
    const auto semRes = sema.analyze(program);
    CodeGenerator gen;
    gen.setSemantics(semRes);
    return gen.generate(program);
}

} // namespace gwbasic::phase_log_helpers
