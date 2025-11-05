// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>

namespace gwbasic {

/*
 * Function: Compiler::compileStringWithPhaseLogs
 * Inputs:
 *  - source: Complete GW-BASIC source as a single string
 *  - lexLogPath: File to append lexical events (tokens with positions)
 *  - syntaxLogPath: File to append syntax parse events (node, line/col)
 *  - semanticLogPath: File to append semantic events (vars/refs/loops)
 *  - codegenLogPath: File to append IR emission events per AST node
 * Outputs:
 *  - std::string: LLVM IR text for the compiled program
 * Theory of operation:
 *  - Executes the pipeline while enabling detailed logs at the parser and
 *    code generator stages to correlate source to structure and emitted IR.
 */
std::string Compiler::compileStringWithPhaseLogs(const std::string& source,
                                                 const std::string& lexLogPath,
                                                 const std::string& syntaxLogPath,
                                                 const std::string& semanticLogPath,
                                                 const std::string& codegenLogPath) {
    Lexer lex(source);
    lex.setLexLogPath(lexLogPath);
    auto tokens = lex.tokenize();
    Parser parser(std::move(tokens));
    parser.setSyntaxLogPath(syntaxLogPath);
    auto program = parser.parseProgram();
    // Semantic analysis (scope + references + strings)
    SemanticAnalyzer sema;
    sema.setLogPath(semanticLogPath);
    auto semRes = sema.analyze(program);
    CodeGenerator gen;
    if (!codegenLogPath.empty()) gen.setLogPath(codegenLogPath);
    gen.setSemantics(semRes);
    return Compiler::addDefaultTripleIfMissing(gen.generate(program));
}

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
 *  - Reads file contents and forwards to compileStringWithPhaseLogs() so
 *    string- and file-based flows share identical behavior and logging.
 */
std::string Compiler::compileFileWithPhaseLogs(const std::string& path,
                                               const std::string& lexLogPath,
                                               const std::string& syntaxLogPath,
                                               const std::string& semanticLogPath,
                                               const std::string& codegenLogPath) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error(std::string("Unable to open input file: ").append(path));
    // Non-recursive import resolver with an explicit LIFO stack
    struct Frame {
        std::string path;
        gwbasic::Program prog;
        size_t idx{0};
        bool mergeMode{false};
    };

    auto resolvePath = [](const std::string& base, const std::string& rel) -> std::string {
        std::filesystem::path p(rel);
        if (p.is_absolute()) return std::filesystem::weakly_canonical(p).string();
        std::filesystem::path b(base);
        return std::filesystem::weakly_canonical(b.parent_path() / p).string();
    };
    auto canonicalPath = [](const std::string& p) -> std::string {
        return std::filesystem::weakly_canonical(std::filesystem::path(p)).string();
    };
    auto parseFile = [&](const std::string& fpath) -> gwbasic::Program {
        std::ifstream fin(fpath);
        if (!fin) throw std::runtime_error(std::string("Unable to open input file: ").append(fpath));
        Lexer lx(fin);
        // Log only root file tokens to avoid truncation; included files skip lex log to keep simplicity
        auto toks = lx.tokenize();
        Parser p(std::move(toks));
        // Only the root parser gets syntax logging to avoid repeated truncation
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

    // Build composite program by following CHAIN/RUN/MERGE depth-first using a LIFO stack
    std::vector<Frame> stack;
    gwbasic::Program program; // composite result
    std::unordered_map<std::string, std::pair<int,int>> imported; // path -> {base, minLine}
    // Root file: set lex + syntax logs
    {
        // Tokenize root with lex log
        Lexer lex2(in);
        lex2.setLexLogPath(lexLogPath);
        auto toks2 = lex2.tokenize();
        Parser rp(std::move(toks2));
        rp.setSyntaxLogPath(syntaxLogPath);
        Frame root{canonicalPath(path), rp.parseProgram(), 0, false};
        // Register root
        int minRoot = INT_MAX;
        for (const auto& l : root.prog.lines) if (l.number < minRoot) minRoot = l.number;
        imported[root.path] = {0, (minRoot == INT_MAX ? 0 : minRoot)};
        stack.push_back(std::move(root));
    }
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
                    dir = Dir::Merge; incPath = resolvePath(fr.path, mg->filename); hadDirective = true; break;
                }
                if (const auto ch = gwbasic::dyn_cast<gwbasic::ChainStmt>(st.get())) {
                    if (ch->filename.has_value()) { dir = Dir::Chain; incPath = resolvePath(fr.path, *ch->filename); hadDirective = true; break; }
                }
                if (const auto rn = gwbasic::dyn_cast<gwbasic::RunStmt>(st.get())) {
                    if (rn->filename.has_value()) { dir = Dir::Run; incPath = resolvePath(fr.path, *rn->filename); hadDirective = true; break; }
                }
            }
            if (hadDirective && (dir == Dir::Chain || dir == Dir::Run)) {
                const std::string canon = canonicalPath(incPath);
                auto it = imported.find(canon);
                if (it == imported.end()) {
                    gwbasic::Program nextProg = parseFile(canon);
                    int minImported = INT_MAX;
                    for (const auto& l2 : nextProg.lines) if (l2.number < minImported) minImported = l2.number;
                    const int base = (canon == fr.path) ? 0 : static_cast<int>(imported.size()) * 1000 + 1000; // simple incremental base
                    for (auto& l2 : nextProg.lines) l2.number += base;
                    imported[canon] = {base, (minImported == INT_MAX ? 0 : minImported)};
                    stack.push_back(Frame{canon, std::move(nextProg), 0, /*mergeMode*/ false});
                    it = imported.find(canon);
                }
                // Patch target line respecting explicit line when given
                if (dir == Dir::Chain) {
                    for (auto& st : ln.statements) {
                        if (auto ch = gwbasic::dyn_cast<gwbasic::ChainStmt>(st.get())) {
                            const int base = it->second.first;
                            const int first = it->second.second;
                            const int tgt = ch->targetLine.has_value() ? *ch->targetLine : first;
                            ch->targetLine = base + tgt; break;
                        }
                    }
                } else {
                    for (auto& st : ln.statements) {
                        if (auto rn = gwbasic::dyn_cast<gwbasic::RunStmt>(st.get())) {
                            const int base = it->second.first;
                            const int first = it->second.second;
                            const int tgt = rn->targetLine.has_value() ? *rn->targetLine : first;
                            rn->targetLine = base + tgt; break;
                        }
                    }
                }
                // Keep the directive line
                replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
                continue;
            } else if (hadDirective && dir == Dir::Merge) {
                const std::string canon = canonicalPath(incPath);
                gwbasic::Program mprog = parseFile(canon);
                for (auto& ml : mprog.lines) replaceOrAppendLine(program, std::move(ml), /*replace*/ true);
                continue;
            }
            replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
        }
    }
    // Semantic analysis (scope + references + strings)
    SemanticAnalyzer sema;
    sema.setLogPath(semanticLogPath);
    auto semRes = sema.analyze(program);
    CodeGenerator gen;
    if (!codegenLogPath.empty()) gen.setLogPath(codegenLogPath);
    gen.setSemantics(semRes);
    return Compiler::addDefaultTripleIfMissing(gen.generate(program));
}

} // namespace gwbasic
