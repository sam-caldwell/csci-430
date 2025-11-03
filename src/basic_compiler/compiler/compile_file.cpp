// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include <fstream>
#include <sstream>

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
    struct Frame { std::string path; gwbasic::Program prog; size_t idx{0}; bool mergeMode{false}; bool clearOnEnter{false}; };
    auto resolvePath = [](const std::string& base, const std::string& rel) -> std::string {
        namespace fs = std::filesystem;
        fs::path p(rel); if (p.is_absolute()) return p.string(); fs::path b(base); return (b.parent_path() / p).string();
    };
    auto parseOne = [&](const std::string& fpath) -> gwbasic::Program {
        std::ifstream fin(fpath); if (!fin) throw std::runtime_error(std::string("Unable to open input file: ").append(fpath));
        Lexer lx(fin); auto toks = lx.tokenize(); Parser p(std::move(toks)); return p.parseProgram();
    };
    auto replaceOrAppendLine = [](gwbasic::Program& dst, gwbasic::Line&& line, bool replace) {
        if (replace) { for (auto& dl : dst.lines) { if (dl.number == line.number) { dl = std::move(line); return; } } }
        dst.lines.push_back(std::move(line));
    };
    // Seed root
    Lexer lex(in); auto tokens = lex.tokenize(); Parser parser(std::move(tokens)); gwbasic::Program root = parser.parseProgram();
    std::vector<Frame> stack; stack.push_back(Frame{path, std::move(root), 0, false, false});
    int nextBase = 1000;
    gwbasic::Program program; // composite
    while (!stack.empty()) {
        Frame fr = std::move(stack.back()); stack.pop_back(); if (fr.clearOnEnter) program.lines.clear();
        for (; fr.idx < fr.prog.lines.size(); ++fr.idx) {
            gwbasic::Line& ln = fr.prog.lines[fr.idx]; bool directiveFound = false; std::string incPath; enum class Dir { None, Merge, Chain, Run } dir{Dir::None};
            for (const auto& st : ln.statements) {
                if (const auto mg = gwbasic::dyn_cast<gwbasic::MergeStmt>(st.get())) { dir = Dir::Merge; incPath = resolvePath(fr.path, mg->filename); directiveFound = true; break; }
                if (const auto ch = gwbasic::dyn_cast<gwbasic::ChainStmt>(st.get())) { if (ch->filename.has_value()) { dir = Dir::Chain; incPath = resolvePath(fr.path, *ch->filename); directiveFound = true; break; } }
                if (const auto rn = gwbasic::dyn_cast<gwbasic::RunStmt>(st.get())) { if (rn->filename.has_value()) { dir = Dir::Run; incPath = resolvePath(fr.path, *rn->filename); directiveFound = true; break; } }
            }
            if (directiveFound) {
                Frame next{incPath, parseOne(incPath), 0, (dir == Dir::Merge), (dir == Dir::Run)};
                // For CHAIN: keep CHAIN line and branch to imported's first line; avoid duplicate line numbers by renumbering import block
                if (dir == Dir::Chain) {
                    int base = nextBase; nextBase += 1000;
                    int minImported = INT_MAX;
                    for (auto& l2 : next.prog.lines) { if (l2.number < minImported) minImported = l2.number; l2.number += base; }
                    // Patch CHAIN target to branch to first imported line
                    for (auto& st : ln.statements) {
                        if (auto ch = gwbasic::dyn_cast<gwbasic::ChainStmt>(st.get())) { ch->targetLine = base + minImported; break; }
                    }
                    // Append the CHAIN line itself to composite before following include
                    replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
                }
                // For RUN: clear composite on enter to emulate restart
                // For MERGE: replace matching lines
                stack.push_back(std::move(next));
                break;
            }
            replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
        }
    }
    // Keep semantics integration consistent with compileString
    SemanticAnalyzer sema; auto res = sema.analyze(program);
    CodeGenerator gen; gen.setSemantics(res);
    return Compiler::addDefaultTripleIfMissing(gen.generate(program));
}

} // namespace gwbasic
