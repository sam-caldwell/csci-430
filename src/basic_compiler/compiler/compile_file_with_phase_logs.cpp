// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/Compiler.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/util/TransparentSVHasher.h"
#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace gwbasic {

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
 *  - Reads file contents and forwards to compileStringWithPhaseLogs, so
 *    string- and file-based flows share identical behavior and logging.
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
// NOLINTBEGIN(bugprone-easily-swappable-parameters)
std::string Compiler::compileFileWithPhaseLogs(const std::string& path, // NOLINT(readability-function-size)
                                               const std::string& lexLogPath,
                                               const std::string& syntaxLogPath,
                                               const std::string& semanticLogPath,
                                               const std::string& codegenLogPath) {

    // Helper: processing frame for depth-first import resolution (local)
    struct Frame { std::string path; gwbasic::Program prog; size_t idx{0}; bool mergeMode{false}; };

    // Root: tokenize with logs and initialize import table
    std::string rootCanon; int minRoot = 0;

    gwbasic::Program rootProg = phase_log_helpers::tokenizeRootWithLogs(path, lexLogPath, syntaxLogPath, rootCanon, minRoot);

    std::unordered_map<std::string, std::pair<int,int>, gwbasic::TransparentSVHasher, std::equal_to<>> imported;

    imported[rootCanon] = {0, minRoot};

    std::vector<Frame> stack;

    stack.emplace_back(rootCanon, std::move(rootProg), 0, false);

    gwbasic::Program program; // composite result
    while (!stack.empty()) {
        // ReSharper disable once CppUseStructuredBinding
        auto& frame = stack.back();
        if (frame.idx >= frame.prog.lines.size()) { stack.pop_back(); continue; }

        using enum gwbasic::phase_log_helpers::Dir;
        gwbasic::Line& lineObj = frame.prog.lines[frame.idx++];
        auto dir{None};
        std::string incPath;
        const bool foundDirective = phase_log_helpers::detectDirective(lineObj, frame.path, dir, incPath);

        if (foundDirective && (dir == Chain || dir == Run)) {
            if (gwbasic::phase_log_helpers::ImportedProg importedProg;
                gwbasic::phase_log_helpers::processChainRun(frame.path, incPath, dir == Chain, imported, lineObj, importedProg)
            ) {
                stack.emplace_back(importedProg.path, std::move(importedProg.prog), 0, false);
            }
            phase_log_helpers::replaceOrAppendLine(program, std::move(lineObj), frame.mergeMode);
            continue;
        }

        if (foundDirective && dir == Merge) {
            gwbasic::phase_log_helpers::processMerge(incPath, program);
            continue;
        }

        phase_log_helpers::replaceOrAppendLine(program, std::move(lineObj), frame.mergeMode);
    }
    if (gMetrics != nullptr) {
        gMetrics->recordParsedSnapshot(program);
        gMetrics->setAnalyzeOnly(true);
        gwbasic::AstOptimizer::optimize(program);
        gMetrics->setAnalyzeOnly(false);
        gMetrics->recordAfterSemanticsSnapshot(program);
    }
    auto irBody = phase_log_helpers::generateIRWithLogs(program, semanticLogPath, codegenLogPath);
    if (gMetrics != nullptr) { gMetrics->setIrInstructionCount(Metrics::countIrInstructions(irBody)); }
    return Compiler::addDefaultTripleIfMissing(irBody);
}
// NOLINTEND(bugprone-easily-swappable-parameters)

} // namespace gwbasic
