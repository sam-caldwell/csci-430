// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/Compiler.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/util/TransparentSVHasher.h"

#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

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
// NOLINTBEGIN(readability-function-size)
std::string Compiler::compileFile(const std::string& path) {
    using namespace gwbasic::phase_log_helpers;
    // Frame for depth-first import resolution (no logs)
    struct Frame { std::string path; gwbasic::Program prog; std::size_t idx{0}; bool mergeMode{false}; };

    // Parse root and seed import table
    std::string rootCanon; int minRoot = 0;
    gwbasic::Program rootProg = tokenizeRootNoLogs(path, rootCanon, minRoot);
    std::unordered_map<std::string, std::pair<int,int>, gwbasic::TransparentSVHasher, std::equal_to<>> imported;
    imported[rootCanon] = {0, minRoot};

    std::vector<Frame> stack;
    stack.emplace_back(rootCanon, std::move(rootProg), 0, false);

    gwbasic::Program program; // composite
    while (!stack.empty()) {
        // ReSharper disable once CppUseStructuredBinding
        auto& frame = stack.back();
        if (frame.idx >= frame.prog.lines.size()) { stack.pop_back(); continue; }

        using enum Dir;
        gwbasic::Line& lineObj = frame.prog.lines[frame.idx++];
        auto dir{None};
        std::string incPath;
        const bool hasDirective = detectDirective(lineObj, frame.path, dir, incPath);

        if (hasDirective && (dir == Chain || dir == Run)) {
            if (ImportedProg importedProg; processChainRun(frame.path, incPath, dir == Chain, imported, lineObj, importedProg)) {
                stack.emplace_back(importedProg.path, std::move(importedProg.prog), 0, false);
            }
            replaceOrAppendLine(program, std::move(lineObj), frame.mergeMode);
            continue;
        }

        if (hasDirective && dir == Merge) {
            processMerge(incPath, program);
            continue;
        }

        replaceOrAppendLine(program, std::move(lineObj), frame.mergeMode);
    }
    if (gMetrics != nullptr) {
        gMetrics->recordParsedSnapshot(program);
        gMetrics->setAnalyzeOnly(true);
        gwbasic::AstOptimizer::optimize(program);
        gMetrics->setAnalyzeOnly(false);
        gMetrics->recordAfterSemanticsSnapshot(program);
    }
    // Keep semantics integration consistent with compileString
    SemanticAnalyzer sema;
    sema.setStrictControlFlow(false);
    auto res = sema.analyze(program);
    CodeGenerator gen;
    gen.setSemantics(res);
    auto irText = gen.generate(program);
    if (gMetrics != nullptr) {
        gMetrics->setIrInstructionCount(Metrics::countIrInstructions(irText));
    }
    return Compiler::addDefaultTripleIfMissing(irText);
}
// NOLINTEND(readability-function-size)

} // namespace gwbasic
