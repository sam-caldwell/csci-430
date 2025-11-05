// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>

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

    std::ifstream in(path);
    if (!in) throw std::runtime_error(std::string("Unable to open input file: ").append(path));
    // Depth-first non-recursive import resolution (no logs)
    struct Frame { std::string path; gwbasic::Program prog; size_t idx{0}; bool mergeMode{false}; };
    auto resolvePath = [](const std::string& base, const std::string& rel) -> std::string {
        std::filesystem::path p(rel);
        if (p.is_absolute()) return std::filesystem::weakly_canonical(p).string();
        std::filesystem::path b(base);
        return std::filesystem::weakly_canonical(b.parent_path() / p).string();
    };
    auto canonicalPath = [](const std::string& p) -> std::string {
        return std::filesystem::weakly_canonical(std::filesystem::path(p)).string();
    };
    auto parseOne = [&](const std::string& fpath) -> gwbasic::Program {
        std::ifstream fin(fpath);
        if (!fin) throw std::runtime_error(std::string("Unable to open input file: ").append(fpath));
        Lexer lx(fin);
        auto toks = lx.tokenize();
        Parser p(std::move(toks));
        return p.parseProgram();
    };
    auto replaceOrAppendLine = [](gwbasic::Program& dst, gwbasic::Line&& line, bool replace) {
        if (replace) {
            for (auto& dl : dst.lines) {
                if (dl.number == line.number) { dl = std::move(line); return; }
            }
        }
        dst.lines.push_back(std::move(line));
    };

    // Seed root
    Lexer lex(in);
    auto tokens = lex.tokenize();
    Parser parser(std::move(tokens));
    gwbasic::Program root = parser.parseProgram();
    std::vector<Frame> stack;
    stack.push_back(Frame{canonicalPath(path), std::move(root), 0, false});

    // Track imported sources to avoid infinite recursion and to map line renumbering
    struct ImportInfo { int base{0}; int minLine{0}; };
    std::unordered_map<std::string, ImportInfo> imported; // canonical path -> mapping

    // Register root program mapping (no renumbering)
    {
        int minRoot = INT_MAX;
        for (const auto& l : stack.back().prog.lines) if (l.number < minRoot) minRoot = l.number;
        imported[stack.back().path] = ImportInfo{0, (minRoot == INT_MAX ? 0 : minRoot)};
    }

    int nextBase = 1000;
    gwbasic::Program program; // composite
    while (!stack.empty()) {
        Frame fr = std::move(stack.back());
        stack.pop_back();
        for (; fr.idx < fr.prog.lines.size(); ++fr.idx) {
            gwbasic::Line& ln = fr.prog.lines[fr.idx];
            bool hadDirective = false;
            std::string incPath;
            enum class Dir { None, Merge, Chain, Run } dir{Dir::None};
            for (const auto& st : ln.statements) {
                if (const auto mg = gwbasic::dyn_cast<gwbasic::MergeStmt>(st.get())) {
                    dir = Dir::Merge;
                    incPath = resolvePath(fr.path, mg->filename);
                    hadDirective = true;
                    break;
                }
                if (const auto ch = gwbasic::dyn_cast<gwbasic::ChainStmt>(st.get())) {
                    if (ch->filename.has_value()) {
                        dir = Dir::Chain;
                        incPath = resolvePath(fr.path, *ch->filename);
                        hadDirective = true;
                        break;
                    }
                }
                if (const auto rn = gwbasic::dyn_cast<gwbasic::RunStmt>(st.get())) {
                    if (rn->filename.has_value()) {
                        dir = Dir::Run;
                        incPath = resolvePath(fr.path, *rn->filename);
                        hadDirective = true;
                        break;
                    }
                }
            }

            if (hadDirective && (dir == Dir::Chain || dir == Dir::Run)) {
                // Ensure import mapping exists for the target; if new, parse and assign a base
                const std::string canon = canonicalPath(incPath);
                auto it = imported.find(canon);
                if (it == imported.end()) {
                    // Parse and renumber imported program
                    gwbasic::Program nextProg = parseOne(canon);
                    int minImported = INT_MAX;
                    for (const auto& l2 : nextProg.lines) if (l2.number < minImported) minImported = l2.number;
                    const int base = (canon == fr.path) ? 0 : nextBase; // self-chain to same file: keep base 0
                    if (canon != fr.path) nextBase += 1000;
                    for (auto& l2 : nextProg.lines) l2.number += base;
                    imported[canon] = ImportInfo{base, (minImported == INT_MAX ? 0 : minImported)};
                    // Push included file for full processing (depth-first), preserving mergeMode=false here
                    stack.push_back(Frame{canon, std::move(nextProg), 0, /*mergeMode*/ false});
                    it = imported.find(canon);
                }
                // Patch target line on this statement respecting specified target if any
                if (dir == Dir::Chain) {
                    for (auto& st : ln.statements) {
                        if (auto ch = gwbasic::dyn_cast<gwbasic::ChainStmt>(st.get())) {
                            const int base = it->second.base;
                            const int first = it->second.minLine;
                            const int tgt = ch->targetLine.has_value() ? *ch->targetLine : first;
                            ch->targetLine = base + tgt;
                            break;
                        }
                    }
                } else if (dir == Dir::Run) {
                    for (auto& st : ln.statements) {
                        if (auto rn = gwbasic::dyn_cast<gwbasic::RunStmt>(st.get())) {
                            const int base = it->second.base;
                            const int first = it->second.minLine;
                            const int tgt = rn->targetLine.has_value() ? *rn->targetLine : first;
                            rn->targetLine = base + tgt;
                            break;
                        }
                    }
                }
                // Append the directive line to composite and continue scanning this file
                replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
                continue;
            } else if (hadDirective && dir == Dir::Merge) {
                // MERGE: inline file contents, replacing duplicate line numbers
                const std::string canon = canonicalPath(incPath);
                gwbasic::Program mprog = parseOne(canon);
                for (auto& ml : mprog.lines) replaceOrAppendLine(program, std::move(ml), /*replace*/ true);
                // Keep the MERGE line itself out of the output program
                continue;
            }

            // Normal line: adopt into composite
            replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
        }
    }
    // Keep semantics integration consistent with compileString
    SemanticAnalyzer sema; auto res = sema.analyze(program);
    CodeGenerator gen; gen.setSemantics(res);
    return Compiler::addDefaultTripleIfMissing(gen.generate(program));
}

} // namespace gwbasic
