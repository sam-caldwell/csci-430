// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <cstdio>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

namespace gwbasic {

/**
 * Compiler: High-level façade that runs the pipeline (lex → parse → codegen).
 *
 * Purpose:
 *  - Convenience helpers to compile from strings or files to IR text.
 */
class Compiler {
public:
    /**
     * compileString: Compile a GW-BASIC program string to LLVM IR.
     *
     * Inputs:
     *  - source: Program text.
     *
     * Outputs:
     *  - std::string: LLVM IR text (.ll).
     */
    static std::string compileString(const std::string& source) {
        Lexer lex(source);
        auto tokens = lex.tokenize();
        Parser parser(std::move(tokens));
        auto program = parser.parseProgram();
        CodeGenerator gen;
        // Provide semantic info to avoid duplicate collection
        SemanticAnalyzer sema;
        auto res = sema.analyze(program);
        gen.setSemantics(res);
        return addDefaultTripleIfMissing(gen.generate(program));
    }

    /**
     * compileFile: Compile a GW-BASIC source file to LLVM IR.
     *
     * Inputs:
     *  - path: Filesystem path to a .bas file.
     *
     * Outputs:
     *  - std::string: LLVM IR text (.ll).
     */
    static std::string compileFile(const std::string& path);

    /**
     * compileStringWithLog: Compile a GW-BASIC program and emit a codegen log.
     * Inputs:
     *  - source: Program text
     *  - logPath: Path to write a human-readable codegen log
     * Outputs:
     *  - std::string: LLVM IR text (.ll)
     */
    static std::string compileStringWithLog(const std::string& source, const std::string& logPath);

    /**
     * compileFileWithLog: Compile a source file and emit a codegen log.
     * Inputs:
     *  - path: Source file path
     *  - logPath: Log file path
     * Outputs:
     *  - std::string: LLVM IR text (.ll)
     */
    static std::string compileFileWithLog(const std::string& path, const std::string& logPath);

    /** Compile with phase logs: lex + syntax + semantic (+ optional codegen). */
    static std::string compileStringWithPhaseLogs(const std::string& source,
                                                  const std::string& lexLogPath,
                                                  const std::string& syntaxLogPath,
                                                  const std::string& semanticLogPath,
                                                  const std::string& codegenLogPath);

    static std::string compileFileWithPhaseLogs(const std::string& path,
                                                const std::string& lexLogPath,
                                                const std::string& syntaxLogPath,
                                                const std::string& semanticLogPath,
                                                const std::string& codegenLogPath);

    /** Compile with AST optimization prior to codegen. */
    static std::string compileStringOptimized(const std::string& source);

private:
    // Prefix IR with clang's effective -cc1 triple for IR compilation to avoid override warnings
    static std::string addDefaultTripleIfMissing(const std::string& ir) {
        // If IR already declares a target triple, keep it
        if (ir.find("target triple =") != std::string::npos) return ir;
        // Ask the system clang how it would invoke cc1 for IR and parse the -triple argument
        std::string triple;
        const char* cmd = "clang -### -S -x ir - -o /dev/null 2>&1";
        if (FILE* pipe = popen(cmd, "r")) {
            char buf[256];
            std::string out;
            while (size_t n = fread(buf, 1, sizeof(buf), pipe)) out.append(buf, buf + n);
            pclose(pipe);
            auto pos = out.find("\"-triple\"");
            if (pos != std::string::npos) {
                auto q1 = out.find('"', pos + 9);
                if (q1 != std::string::npos) {
                    auto q2 = out.find('"', q1 + 1);
                    if (q2 != std::string::npos && q2 > q1 + 1) triple = out.substr(q1 + 1, q2 - (q1 + 1));
                }
            }
        }
        if (triple.empty()) return ir;
        std::ostringstream out;
        out << "target triple = \"" << triple << "\"\n\n" << ir;
        return out.str();
    }
};

} // namespace gwbasic
