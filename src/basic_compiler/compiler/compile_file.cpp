// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/Compiler.h"
#include <unordered_map>
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/util/TransparentSVHasher.h"
#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/opt/AstOptimizer.h"

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
    using namespace gwbasic::phase_log_helpers;
    // Frame for depth-first import resolution (no logs)
    struct Frame { std::string path; gwbasic::Program prog; size_t idx{0}; bool mergeMode{false}; };

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
        auto& fr = stack.back();
        if (fr.idx >= fr.prog.lines.size()) { stack.pop_back(); continue; }

        using enum Dir;
        gwbasic::Line& ln = fr.prog.lines[fr.idx++];
        auto dir{None};
        std::string incPath;
        bool had = detectDirective(ln, fr.path, dir, incPath);

        if (had && (dir == Chain || dir == Run)) {
            if (ImportedProg ip; processChainRun(fr.path, incPath, dir == Chain, imported, ln, ip)) {
                stack.emplace_back(ip.path, std::move(ip.prog), 0, false);
            }
            replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
            continue;
        }

        if (had && dir == Merge) {
            processMerge(incPath, program);
            continue;
        }

        replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
    }
    if (gMetrics) {
        gMetrics->recordParsedSnapshot(program);
        gMetrics->setAnalyzeOnly(true);
        gwbasic::AstOptimizer::optimize(program);
        gMetrics->setAnalyzeOnly(false);
        gMetrics->recordAfterSemanticsSnapshot(program);
    }
    // Keep semantics integration consistent with compileString
    SemanticAnalyzer sema; sema.setStrictControlFlow(false); auto res = sema.analyze(program);
    CodeGenerator gen; gen.setSemantics(res);
    auto ir = gen.generate(program);
    if (gMetrics) gMetrics->setIrInstructionCount(Metrics::countIrInstructions(ir));
    return Compiler::addDefaultTripleIfMissing(ir);
}

} // namespace gwbasic
