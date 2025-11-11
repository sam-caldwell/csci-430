// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_COMPILER_COMPILER_H
#define BASIC_COMPILER_COMPILER_COMPILER_H

#include <string>

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
     * Function: Compiler::compileString
     * Purpose:
     *  - Compile a GW-BASIC program string to LLVM IR.
     * Inputs:
     *  - source: Program text
     * Outputs:
     *  - std::string: LLVM IR text (.ll)
     * Theory of operation:
     *  - Tokenizes and parses the source, runs semantics, and generates IR
     *    with a default target triple header when missing.
     */
    static std::string compileString(const std::string& source);

    /**
     * Function: Compiler::compileFile
     * Purpose:
     *  - Compile a GW-BASIC source file to LLVM IR.
     * Inputs:
     *  - path: Filesystem path to a .bas file
     * Outputs:
     *  - std::string: LLVM IR text (.ll)
     */
    static std::string compileFile(const std::string& path);

    /**
     * Function: Compiler::compileStringWithLog
     * Purpose:
     *  - Compile a GW-BASIC program string and emit a codegen log.
     * Inputs:
     *  - source: Program text
     *  - logPath: Path to write a human-readable codegen log
     * Outputs:
     *  - std::string: LLVM IR text (.ll)
     */
    static std::string compileStringWithLog(const std::string& source, const std::string& logPath);

    /**
     * Function: Compiler::compileFileWithLog
     * Purpose:
     *  - Compile a source file and emit a codegen log.
     * Inputs:
     *  - path: Source file path
     *  - logPath: Log file path
     * Outputs:
     *  - std::string: LLVM IR text (.ll)
     */
    static std::string compileFileWithLog(const std::string& path, const std::string& logPath);

    /**
     * Function: Compiler::compileStringWithPhaseLogs
     * Purpose:
     *  - Compile program string with phase logs (lexer, syntax, semantics,
     *    and optional codegen log).
     * Inputs:
     *  - source: Program text
     *  - lexLogPath: Destination for lexer log
     *  - syntaxLogPath: Destination for parser/syntax log
     *  - semanticLogPath: Destination for semantics log
     *  - codegenLogPath: Destination for codegen log
     * Outputs:
     *  - std::string: LLVM IR text (.ll)
     */
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    static std::string compileStringWithPhaseLogs(const std::string& source,
                                                  const std::string& lexLogPath,
                                                  const std::string& syntaxLogPath,
                                                  const std::string& semanticLogPath,
                                                  const std::string& codegenLogPath);

    /**
     * Function: Compiler::compileFileWithPhaseLogs
     * Purpose:
     *  - Compile a source file with phase logs (lexer, syntax, semantics,
     *    and optional codegen log).
     * Inputs:
     *  - path: Path to .bas source file
     *  - lexLogPath: Destination for lexer log
     *  - syntaxLogPath: Destination for parser/syntax log
     *  - semanticLogPath: Destination for semantics log
     *  - codegenLogPath: Destination for codegen log
     * Outputs:
     *  - std::string: LLVM IR text (.ll)
     */
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    static std::string compileFileWithPhaseLogs(const std::string& path,
                                                const std::string& lexLogPath,
                                                const std::string& syntaxLogPath,
                                                const std::string& semanticLogPath,
                                                const std::string& codegenLogPath);

    /**
     * Function: Compiler::compileStringOptimized
     * Purpose:
     *  - Compile program string with AST optimization prior to codegen.
     * Inputs:
     *  - source: Program text
     * Outputs:
     *  - std::string: Optimized LLVM IR text (.ll)
     */
    static std::string compileStringOptimized(const std::string& source);

private:
    /**
     * Function: Compiler::addDefaultTripleIfMissing
     * Purpose:
     *  - Prefix IR with clang's effective -cc1 target triple when missing.
     * Inputs:
     *  - ir: LLVM IR text
     * Outputs:
     *  - std::string: IR with leading target triple if absent
     * Theory of operation:
     *  - Runs 'clang -### -S -x ir - -o /dev/null' and parses the emitted
     *    -triple argument; if found, prepends a target triple header.
     */
    static std::string addDefaultTripleIfMissing(const std::string& ir_text);
};

} // namespace gwbasic

#endif // BASIC_COMPILER_COMPILER_COMPILER_H
