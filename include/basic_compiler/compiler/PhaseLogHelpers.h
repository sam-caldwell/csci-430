// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_COMPILER_PHASELOGHELPERS_H
#define BASIC_COMPILER_COMPILER_PHASELOGHELPERS_H

#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/util/TransparentSVHasher.h"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>

namespace gwbasic::phase_log_helpers {

/**
 * Enum: Dir
 * Purpose:
 *  - Classify import-like directives discovered in a line.
 * Members: None, Merge, Chain, Run
 */
enum class Dir : std::uint8_t { None, Merge, Chain, Run };

/**
 * Function: resolvePath
 * Purpose:
 *  - Resolve a possibly relative path against a base file path.
 * Inputs:
 *  - baseFile: Full path to the current source file
 *  - rel: Relative or absolute import path
 * Outputs:
 *  - std::string: Canonicalized combined path string
 */
std::string resolvePath(const std::string& baseFile, const std::string& rel);

/**
 * Function: canonicalPath
 * Purpose:
 *  - Produce a weakly canonical path suitable for path map keys.
 * Inputs:
 *  - p: Path string (relative or absolute)
 * Outputs:
 *  - std::string: Weakly canonical absolute path
 */
std::string canonicalPath(const std::string& path);

/**
 * Function: replaceOrAppendLine
 * Purpose:
 *  - Replace a line in a Program when its number matches, otherwise append.
 * Inputs:
 *  - dst: Program to mutate
 *  - line: R-value line to store
 *  - replace: Whether to replace on duplicate line numbers
 * Outputs:
 *  - void
 */
void replaceOrAppendLine(gwbasic::Program& dst, gwbasic::Line&& line, bool replace);

/**
 * Function: detectDirective
 * Purpose:
 *  - Inspect a line for MERGE/CHAIN/RUN directives and compute include path.
 * Inputs:
 *  - ln: Line to inspect
 *  - curPath: Canonical path to the current source file
 *  - dir/outIncPath: Outputs: discovered directive and resolved include path
 * Outputs:
 *  - bool: true if a directive was found; false otherwise
 */
bool detectDirective(const gwbasic::Line& lineObj,
                     const std::string& curPath,
                     Dir& dir,
                     std::string& outIncPath);

/**
 * Function: parseFileNoLogs
 * Purpose:
 *  - Read and parse a GW-BASIC source file without enabling any logs.
 * Inputs:
 *  - fpath: Filesystem path to source
 * Outputs:
 *  - Program: Parsed AST
 * Throws:
 *  - std::runtime_error on file open failure
 */
gwbasic::Program parseFileNoLogs(const std::string& fpath);

/**
 * Function: tokenizeRootWithLogs
 * Purpose:
 *  - Tokenize and parse the root source with lexer/syntax logs enabled.
 * Inputs:
 *  - path: Filesystem path to root source
 *  - lexLogPath/syntaxLogPath: Destination log paths
 * Outputs:
 *  - Program: Parsed AST for the root file
 *  - outCanon: Canonical path to the root file
 *  - outMinLine: Minimum line number in the root program (or 0 if none)
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
gwbasic::Program tokenizeRootWithLogs(const std::string& path,
                                      const std::string& lexLogPath,
                                      const std::string& syntaxLogPath,
                                      std::string& outCanon,
                                      int& outMinLine);

/**
 * Function: tokenizeRootNoLogs
 * Purpose:
 *  - Parse the root source file without emitting any logs and compute its
 *    canonical path and minimum line number.
 * Inputs:
 *  - path: Filesystem path to the root source
 * Outputs:
 *  - Program: Parsed AST for the root file
 *  - outCanon: Canonical path to the root file
 *  - outMinLine: Minimum line number in the root program (or 0 if empty)
 */
gwbasic::Program tokenizeRootNoLogs(const std::string& path,
                                    std::string& outCanon,
                                    int& outMinLine);

// assignBase: see templated overload below for implementation

/**
 * Template: assignBase (heterogeneous map-friendly)
 * Purpose:
 *  - Compute a renumbering base for an imported program using a generic
 *    associative container (supports heterogeneous hash/equality).
 * Notes:
 *  - Matches the non-template behavior: returns 0 when canon == curPath,
 *    otherwise uses imported.size() to assign 1000-incremented bases.
 */
template <class Map>
int assignBase(const std::string& canon,
               const std::string& curPath,
               const Map& imported) {
    if (canon == curPath) { return 0; }
    constexpr int kBaseStep = 1000; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    return (static_cast<int>(imported.size()) * kBaseStep) + kBaseStep;
}

/**
 * Function: renumberProgram
 * Purpose:
 *  - Apply a base offset to all lines in a program and compute its min line.
 * Inputs:
 *  - prog: Program to mutate
 *  - base: Base offset to add to each line number
 * Outputs:
 *  - outMinLine: Minimum line number after renumbering
 */
void renumberProgram(gwbasic::Program& prog, int base, int& outMinLine);

/**
 * Function: generateIRWithLogs
 * Purpose:
 *  - Run semantics and codegen with optional logs and return LLVM IR.
 * Inputs:
 *  - program: Parsed/combined Program AST
 *  - semanticLogPath: Destination for semantic log
 *  - codegenLogPath: Destination for codegen log (optional)
 * Outputs:
 *  - std::string: LLVM IR text
 */
std::string generateIRWithLogs(const gwbasic::Program& program,
                               const std::string& semanticLogPath,
                               const std::string& codegenLogPath);

/**
 * Struct: ImportedProg
 * Purpose:
 *  - Holder for an imported program and its canonical path, for callers
 *    that manage a stack of files to process.
 */
struct ImportedProg { std::string path; gwbasic::Program prog; };

/**
 * Function: ensureImported
 * Purpose:
 *  - Ensure an imported program is present in the 'imported' map. If not,
 *    parse/renumber it, record in the map, and return the program.
 * Inputs:
 *  - canon: Canonical path to the imported file
 *  - curPath: Canonical path to current file
 *  - imported: Path -> {base, minLine} mapping
 * Outputs:
 *  - out: When a new import occurs, populated with path+program
 *  - bool: true if a new import was parsed; false if already known
 */
/**
 * Template: ensureImported (heterogeneous map-friendly)
 * Purpose:
 *  - Generic helper that works with any associative map type exposing
 *    find(key), operator[](key), size(), and mapped_type=std::pair<int,int>.
 */
template <class Map>
bool ensureImported(const std::string& canon,
                    const std::string& curPath,
                    Map& imported,
                    ImportedProg& out) {
    if (imported.find(canon) != imported.end()) { return false; }
    gwbasic::Program nextProg = parseFileNoLogs(canon);
    int minImported = 0;
    const int base = assignBase(canon, curPath, imported);
    renumberProgram(nextProg, base, minImported);
    imported[canon] = {base, minImported};
    out.path = canon;
    out.prog = std::move(nextProg);
    return true;
}

/**
 * Function: patchTargetsForChainOrRun
 * Purpose:
 *  - Patch the target line for CHAIN or RUN statements in a line to the
 *    computed base + first line of the imported program.
 * Inputs:
 *  - ln: Line to patch
 *  - importInfo: {base, minLine} pair for imported program
 *  - isChain: true to patch CHAIN; false to patch RUN
 */
void patchTargetsForChainOrRun(const gwbasic::Line& lineObj,
                               const std::pair<int,int>& importInfo,
                               bool isChain);

/**
 * Function: appendMergeProgramReplacing
 * Purpose:
 *  - Append the lines from 'src' into 'dst', replacing duplicate line numbers.
 * Inputs:
 *  - dst: Destination program
 *  - src: Source program (rvalue)
 */
void appendMergeProgramReplacing(gwbasic::Program& dst, gwbasic::Program&& src);

/**
 * Function: processChainRun
 * Purpose:
 *  - Handle CHAIN/RUN directive logic: ensure import, patch target line.
 * Inputs:
 *  - curPath: Canonical path of current file
 *  - incPath: Resolved include path (may be relative; will be canonicalized)
 *  - isChain: true for CHAIN; false for RUN
 *  - imported: Path -> {base, minLine} mapping with transparent hasher
 *  - ln: Line to patch in-place
 * Outputs:
 *  - out: Populated with imported program if a new import was parsed
 *  - bool: true if a new import occurred; false otherwise
 */
bool processChainRun(const std::string& curPath,
                     const std::string& incPath,
                     bool isChain,
                     std::unordered_map<std::string, std::pair<int,int>, gwbasic::TransparentSVHasher, std::equal_to<>>& imported,
                     gwbasic::Line& lineObj,
                     ImportedProg& out);

/**
 * Function: processMerge
 * Purpose:
 *  - Handle MERGE directive logic: parse and append/replace lines.
 * Inputs:
 *  - incPath: Resolved include path (may be relative; will be canonicalized)
 *  - program: Destination program to mutate
 */
void processMerge(const std::string& incPath, gwbasic::Program& program);

} // namespace gwbasic::phase_log_helpers

#endif // BASIC_COMPILER_COMPILER_PHASELOGHELPERS_H
