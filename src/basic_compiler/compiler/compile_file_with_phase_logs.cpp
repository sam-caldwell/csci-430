// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include <fstream>
#include <sstream>
#include <filesystem>

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
        bool clearOnEnter{false};
    };

    auto resolvePath = [](const std::string& base, const std::string& rel) -> std::string {
        std::filesystem::path p(rel);
        if (p.is_absolute()) return p.string();
        std::filesystem::path b(base);
        return (b.parent_path() / p).string();
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
    // Root file: set lex + syntax logs
    {
        // Tokenize root with lex log
        Lexer lex2(in);
        lex2.setLexLogPath(lexLogPath);
        auto toks2 = lex2.tokenize();
        Parser rp(std::move(toks2));
        rp.setSyntaxLogPath(syntaxLogPath);
        Frame root{path, rp.parseProgram(), 0, false, false};
        stack.push_back(std::move(root));
    }
    while (!stack.empty()) {
        Frame fr = std::move(stack.back()); stack.pop_back();
        if (fr.clearOnEnter) program.lines.clear();
        bool stopHere = false;
        for (; fr.idx < fr.prog.lines.size(); ++fr.idx) {
            gwbasic::Line& ln = fr.prog.lines[fr.idx];
            bool directiveFound = false;
            std::string incPath;
            enum class Dir { None, Merge, Chain, Run } dir{Dir::None};
            for (const auto& st : ln.statements) {
                if (const auto mg = gwbasic::dyn_cast<gwbasic::MergeStmt>(st.get())) {
                    dir = Dir::Merge; incPath = resolvePath(fr.path, mg->filename); directiveFound = true; break;
                }
                if (const auto ch = gwbasic::dyn_cast<gwbasic::ChainStmt>(st.get())) {
                    if (ch->filename.has_value()) { dir = Dir::Chain; incPath = resolvePath(fr.path, *ch->filename); directiveFound = true; break; }
                }
                if (const auto rn = gwbasic::dyn_cast<gwbasic::RunStmt>(st.get())) {
                    if (rn->filename.has_value()) { dir = Dir::Run; incPath = resolvePath(fr.path, *rn->filename); directiveFound = true; break; }
                }
            }
            if (directiveFound) {
                // Stop parsing this file at directive and follow included path depth-first
                Frame next{incPath, parseFile(incPath), 0, /*mergeMode*/ (dir == Dir::Merge), /*clearOnEnter*/ (dir == Dir::Run)};
                stack.push_back(std::move(next));
                stopHere = true;
                break;
            }
            // Normal line: adopt into composite
            replaceOrAppendLine(program, std::move(ln), fr.mergeMode);
        }
        // If no directive encountered and finished file, nothing to push; continue with next frame
        (void)stopHere;
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
