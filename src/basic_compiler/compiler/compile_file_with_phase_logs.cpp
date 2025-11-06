// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/compiler/Compiler.h"
#include "basic_compiler/compiler/PhaseLogHelpers.h"
#include "basic_compiler/util/TransparentSVHasher.h"
#include <string_view>
#include <unordered_map>
#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/opt/AstOptimizer.h"

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
std::string Compiler::compileFileWithPhaseLogs(const std::string& path,
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
        auto& fr = stack.back();
        if (fr.idx >= fr.prog.lines.size()) { stack.pop_back(); continue; }

        using enum gwbasic::phase_log_helpers::Dir;
        gwbasic::Line& ln = fr.prog.lines[fr.idx++];
        auto dir{None};
        std::string incPath;
        bool had = phase_log_helpers::detectDirective(ln, fr.path, dir, incPath);

        if (had && (dir == Chain || dir == Run)) {
            if (gwbasic::phase_log_helpers::ImportedProg ip;
                gwbasic::phase_log_helpers::processChainRun(fr.path, incPath, dir == Chain, imported, ln, ip)
            ) {
                stack.emplace_back(ip.path, std::move(ip.prog), 0, false);
            }
            phase_log_helpers::replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
            continue;
        }

        if (had && dir == Merge) {
            gwbasic::phase_log_helpers::processMerge(incPath, program);
            continue;
        }

        phase_log_helpers::replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
    }
    if (gMetrics) {
        gMetrics->recordParsedSnapshot(program);
        gMetrics->setAnalyzeOnly(true);
        gwbasic::AstOptimizer::optimize(program);
        gMetrics->setAnalyzeOnly(false);
        gMetrics->recordAfterSemanticsSnapshot(program);
    }
    auto irBody = phase_log_helpers::generateIRWithLogs(program, semanticLogPath, codegenLogPath);
    if (gMetrics) gMetrics->setIrInstructionCount(Metrics::countIrInstructions(irBody));
    return Compiler::addDefaultTripleIfMissing(irBody);
}

} // namespace gwbasic
