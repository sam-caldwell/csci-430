// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <map>
#include <ranges>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <format>
#include <utility>
#include <cwchar>
#include <wchar.h>
#include "logger/Logger.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Stmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/NumberExpr.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/VarExpr.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/CallExpr.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/ForStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/AssignStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/PrintStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/ArrayAssignStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/MidAssignStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/OnGotoStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/OnGosubStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/OpenStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/InputStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/GotoStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/GosubStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/EndStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/ReturnStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/RunStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/CommonStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/DataStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/ReadStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/ChainStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/ast/MergeStmt.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "basic_compiler/codegen/CodeGenError.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/Traits.h"
#include "basic_compiler/ast/DefFnStmt.h"

namespace gwbasic {

/**
 * Class: CodeGenerator
 * Purpose:
 *  - Emit textual LLVM IR (.ll) from a validated AST (Program).
 * Inputs:
 *  - program: Parsed program to compile
 * Outputs:
 *  - std::string: Complete LLVM IR module as text
 * Theory of operation:
 *  - Two-phase approach: collect declarations (variables, strings), then
 *    emit module header/globals and lower each line’s statements into IR.
 *  - Logging hooks provide detailed mapping from AST to emitted IR.
 */
class CodeGenerator {
public:
    /**
     * Function: CodeGenerator::CodeGenerator
     * Purpose:
     *  - Construct a code generator with empty internal state.
     * Inputs:
     *  - none
     * Outputs:
     *  - Default-initialized generator ready for use
     */
    CodeGenerator() = default;

    /*
     * Function: CodeGenerator::generate
     * Purpose:
     *  - Convert a validated Program AST to textual LLVM IR (.ll).
     * Inputs:
     *  - program: Parsed AST to lower
     * Outputs:
     *  - std::string: Full LLVM IR module text
     */
    std::string generate(const Program& program);
    /*
     * Function: CodeGenerator::setSemantics
     * Purpose:
     *  - Provide precomputed semantic results (vars/strings/lines/common)
     *    to seed the generator and avoid redundant collection.
     * Inputs:
     *  - r: SemanticAnalyzer::Result with discovered program facts
     * Outputs:
     *  - void (sets internal semantic caches)
     */
    void setSemantics(const SemanticAnalyzer::Result& r);

private:
    // Counters and symbol maps
    /*
     * Property: tempCounter_
     * Purpose:
     *  - Monotonic counter for generating unique SSA temporary names.
     */
    int tempCounter_{0};
    /*
     * Property: strCounter_
     * Purpose:
     *  - Counter for assigning unique ids to string literals.
     */
    int strCounter_{0};
    /*
     * Property: variables_
     * Purpose:
     *  - Set of all variable identifiers referenced/declared in the program.
     */
    std::set<std::string, std::less<>> variables_;
    /*
     * Property: varAllocaName_
     * Purpose:
     *  - Map from variable name to its LLVM alloca symbol name.
     */
    std::map<std::string, std::string, std::less<>> varAllocaName_;
    /*
     * Property: strLiteralId_
     * Purpose:
     *  - Map string literal value to a unique id used for global names.
     */
    std::map<std::string, int, std::less<>> strLiteralId_;
    std::map<std::string, std::vector<int>, std::less<>> arrayDims_{};
    std::map<std::string, std::string, std::less<>> arrayAllocaName_{};
    // OPTION BASE setting (0 default; 1 if OPTION BASE 1 seen)
    int optionBase_{0};
    bool printZones_{false};
    // User-defined functions by uppercase name
    std::map<std::string, const DefFnStmt*, std::less<>> userFunctions_{};
    // Variables determined as string-typed (by suffix or DEFSTR)
    std::set<std::string, std::less<>> semStringVariables_{};
    // Variables numeric kind mapping (non-strings only)
    enum class NumKind { Int16, Long32, Single, Double };
    std::map<std::string, NumKind, std::less<>> semNumericKinds_{};
    /***
     * Property: lineNumbers_
     * Purpose:
     *  - Sorted list of all program line numbers (emission order driver).
     */
    std::vector<int> lineNumbers_;
    /***
     * Property: lineMap_
     * Purpose:
     *  - Map from line number to Line* for quick lookup during emission.
     */
    std::map<int, const Line*> lineMap_;
    /***
     * Property: currentLine_
     * Purpose:
     *  - Currently emitting line number for logging context.
     */
    int currentLine_{0};
    /***
     * Property: needsRndHelper_
     * Purpose:
     *  - Flag indicating whether RND(x) helper function must be emitted.
     */
    bool needsRndHelper_{false};
    bool needsColor_{false};
    // Whether STOP appears anywhere (to emit break message global)
    bool needsBreakMsg_{false};
    // Inline call-time substitution bindings (stack of name->SSA value)
    std::vector<std::map<std::string, std::string>> bindingStack_{};
    // Optional semantic input
    /***
     * Property: semProvided_
     * Purpose:
     *  - Whether setSemantics() was called to seed generator state.
     */
    bool semProvided_{false};
    /***
     * Property: semVariables_
     * Purpose:
     *  - Variables set provided by semantic analysis.
     */
    std::set<std::string, std::less<>> semVariables_{};
    /***
     * Property: semStrings_
     * Purpose:
     *  - String literals set provided by semantics.
     */
    std::set<std::string, std::less<>> semStrings_{};
    /***
     * Property: semLineNumbers_
     * Purpose:
     *  - Line numbers set provided by semantics.
     */
    std::set<int> semLineNumbers_{};
    /***
     * Property: semCommonVariables_
     * Purpose:
     *  - Variables marked as COMMON by semantics.
     */
    std::set<std::string, std::less<>> semCommonVariables_{};
    /***
     * Property: commonVariables_
     * Purpose:
     *  - Variables declared as COMMON in the current program.
     */
    std::set<std::string, std::less<>> commonVariables_{};
    /***
     * Property: commonBeforeLine_
     * Purpose:
     *  - Snapshot of COMMON variables in effect before each line number to
     *    drive CHAIN scoping behavior.
     */
    std::map<int, std::set<std::string, std::less<>>> commonBeforeLine_{};
    // Snapshot of variables seen before each line (in source order)
    std::map<int, std::set<std::string, std::less<>>> varsBeforeLine_{};
    // Snapshot of arrays seen (DIM'd or referenced) before each line
    std::map<int, std::set<std::string, std::less<>>> arraysBeforeLine_{};
    // For error handlers: map trap start line -> first non-handler line after the
    // handler region (i.e., the line following the first line containing RESUME)
    std::map<int, int> handlerSkipAfter_{};
    // Mapping from 1000-based line region base (e.g., 0, 1000, 2000, ...)
    // to the DATA table index at the start of that region. Used to reset
    // the DATA pointer on CHAIN to a new program segment.
    std::map<int, int> regionDataStartIdx_{};
    // DATA items as string literal ids in program order
    std::vector<int> dataLiteralIds_{};
    // DATA item kind markers (1 when originally quoted string, 0 when numeric)
    std::vector<uint8_t> dataIsString_{};
    // DATA numeric values (double) for numeric items; undefined for string items
    std::vector<double> dataNumValues_{};

    // Phase logging via ostream-based logger
    logger::Logger codegenLogger_{};
    logger::Logger semLogger_{};
    // Optional: a syntax logger accessor exists for unified interface
    logger::Logger syntaxLogger_{};

    // Naming helpers
    /**
     * Function: CodeGenerator::nextTemp
     * Purpose:
     *  - Allocate a unique SSA temporary name for IR emission.
     * Outputs:
     *  - std::string: New temporary name (e.g., "%t3")
     */
    std::string nextTemp();
    /**
     * Function: CodeGenerator::globalStringName
     * Purpose:
     *  - Derive a stable global symbol name for a string literal id.
     * Inputs:
     *  - id: Unique integer for the string literal
     * Outputs:
     *  - std::string: Global symbol (e.g., "@.str.5")
     */
    static std::string globalStringName(int id);
    /**
     * Function: CodeGenerator::lineLabelName
     * Purpose:
     *  - Build a canonical basic block label for a BASIC line number.
     * Inputs:
     *  - ln: BASIC source line number
     * Outputs:
     *  - std::string: Label (e.g., "line100")
     */
    static std::string lineLabelName(int ln);
    /** Label for re-executing a specific statement index within a line. 1-based index. */
    static std::string resumeLabelName(int ln, int stmtIndex);
    /** Label for resuming at the statement after a given index within a line. 1-based index. */
    static std::string resumeNextLabelName(int ln, int stmtIndex);

    // Byte escaping helper for IR string literals (one-function-per-file)
    static void appendEscapedByte(std::string& out, unsigned char c);

    // Declaration collection
    /** Collect declarations, variables, strings, and line ordering. */
    // collectDecls() helpers (one per file) to reduce nesting
    void collectDecls(const Program& program);

    void cdAccumulateFromStatement( // NOLINT(bugprone-easily-swappable-parameters)
        const Stmt* stmt,
        std::set<std::string, std::less<>>& accumCommon,
        std::set<std::string, std::less<>>& accumVars,
        std::set<std::string, std::less<>>& accumArrays);

    void cdGatherLinesAndDeletes(const Program& program,
                                 std::vector<int>& linesOut,
                                 std::map<int, const Line*>& lineMapOut,
                                 bool& printZones,
                                 std::vector<std::pair<int,int>>& deleteRanges,
                                 int& globalMin,
                                 int& globalMax);

    void cdFilterDeletedLines(std::vector<int>& lines,
                              const std::vector<std::pair<int,int>>& deleteRanges);

    void cdCollectVarsIfNoSemantics(const std::vector<int>& lines);
    void cdScanRndAndStop(const std::vector<int>& lines);
    void cdBuildBeforeLineSnapshots(const std::vector<int>& lines);
    void cdSeedFromSemantics();
    void cdAssignInputPromptLiteralIds(const std::vector<int>& lines);
    void cdCollectDataItems(const std::vector<int>& lines);
    void cdCollectDataItemsFromLine(int lineNum);
    // ReSharper disable once CppRedundantElaboratedTypeSpecifier
    void cdCollectDataItemsFromStmt(const struct DataStmt& dataStmt);
    void cdBuildRegionDataStartIdx(const std::vector<int>& lines);
    void cdComputeHandlerSkipAfter(const std::vector<int>& lines);
    void cdCollectTrapTargets(const std::vector<int>& lines, std::set<int>& trapTargets);
    int cdFindLineIndex(const std::vector<int>& lines, int line) const;
    int cdFindResumeEndIdx(const std::vector<int>& lines, int startIdx) const;
    static int cdComputeSkipFromIndices(const std::vector<int>& lines, const std::pair<int,int> &idx);
    void cdMaybeAddTrapTarget(const Stmt* stmt, std::set<int>& trapTargets);
    // Helpers to collect variable/array references for varsBeforeLine_/arraysBeforeLine_
    void collectVarsForBeforeLineFromExpr(const Expr* e, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    void collectVarsForBeforeLineFromStmt(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);

    // Per-kind handlers to reduce complexity; implemented one-per-file
    bool handleAssignBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleArrayAssignBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleIfBlockBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleIfBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleForBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleWhileBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handlePrintBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    auto handleInputBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, const std::set<std::string, std::less<>>* arrays) -> bool;
    bool handleReadBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleDimBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& arrays);
    bool handleSwapBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    static bool handleEraseBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& arrays);
    bool handleWriteBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleOnGotoBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleOnGosubBeforeLine(const Stmt* stmt, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    /** Collect variables/strings referenced by an expression. */
    void collectExprVars(const Expr* e);
    /** Collect variables/strings/COMMON from a statement (recursive). */
    void collectStmtVars(const Stmt* stmt);
    // collectStmtVars() helpers
    void csvHandlePrint(const PrintStmt* p);
    void csvHandleAssign(const AssignStmt* a);
    void csvHandleMidAssign(const MidAssignStmt* m);
    void csvHandleIf(const IfStmt* i);
    void csvHandleFor(const ForStmt* f);
    void csvHandleInput(const InputStmt* in);
    void csvHandleRandomize(const RandomizeStmt* randomizeStmt);
    void csvHandleCommon(const CommonStmt* commonStmt);
    void csvHandleData(const DataStmt* dataStmt);
    void csvHandleRead(const ReadStmt* readStmt);
    void csvHandleOnGoto(const OnGotoStmt* onGotoStmt);
    void csvHandleOnGosub(const OnGosubStmt* onGosubStmt);
    // Lightweight scan for RND usage independent of semantics
    /** Scan expression for RND() usage to enable helper emission. */
    void scanExprForRnd(const Expr* e);
    /** Scan statement (and children) for RND() usage. */
    void scanStmtForRnd(const Stmt* stmt);
    /** Scan statement (and children) for STOP usage. */
    void scanStmtForStop(const Stmt* stmt);

    // Emission helpers
    /** Emit module-level declarations (printf, math, rng helpers). */
    void emitHeader(std::ostringstream& out);
    /** Emit global constants (format strings, literals). */
    void emitGlobals(std::ostringstream& out);
    /** Emit 'main' prologue and entry label. */
    void emitMainPrologue(std::ostringstream& out);

    /** Emit 'main' epilogue and return. */
    static void emitMainEpilogue(std::ostringstream& out);
    /** Emit IR for a single BASIC line block and branch/fallthrough. */
    void emitLineBlock(std::ostringstream& out, const Line& line, int lineIndex, int lastIndex);
    /** Emit a FOR...NEXT loop body/control. */
    void emitFor(std::ostringstream& out, const ForStmt* fs, const std::string& currLineLabel, int& localCounter);
    /** Emit an IF...THEN[/ELSE] structured block. */
    void emitIfBlock(std::ostringstream& out, const IfBlockStmt* ib, const std::string& currLineLabel, int& localCounter);
    /** Emit a WHILE...WEND loop block. */
    void emitWhile(std::ostringstream& out, const WhileStmt* ws, const std::string& currLineLabel, int& localCounter);
    /** Inline a GOSUB target and branch back to a continuation label. */
    void emitSubroutineInline(std::ostringstream& out, int targetLine, const std::string& entryLabel, const std::string& returnLabel);
    /*
     * Function: CodeGenerator::emitSubroutineInlineStatement
     * Purpose: Dispatch a single statement in an inlined subroutine.
     * Returns true if the statement terminates control flow.
     */
    bool emitSubroutineInlineStatement(std::ostringstream& out,
                                       const Stmt* stmt,
                                       std::string_view entryLabel,
                                       const std::string& returnLabel,
                                       int& localCounter);

    // Expression lowering
    /** Lower an expression to SSA value; returns its name. */
    std::string emitExpr(std::ostringstream& out, const Expr* e, [[maybe_unused]] const std::string& currBlockSuffix);
    // Refactored helpers (one-function-per-file) used by emitExpr dispatcher
    std::string emitNumberExpr(const std::ostringstream& out, const struct NumberExpr* num);
    std::string emitVarExpr(std::ostringstream& out, const struct VarExpr* v);
    std::string emitUnaryExpr(std::ostringstream& out, const struct UnaryExpr* u);
    std::string emitBinaryExpr(std::ostringstream& out, const struct BinaryExpr* b);
    std::string emitCallExpr(std::ostringstream& out, const struct CallExpr* call);
    std::string emitStringExpr(std::ostringstream& out, const struct StringExpr* stringExpr);
    // Helper: determine whether an expression is string-typed (for codegen routing)
    bool isStringExpr(const Expr* e) const;
    /** Lower a comparison expression to an i1 predicate value. */
    std::string emitComparison(std::ostringstream& out, const BinaryExpr* c);

    // Utilities
    /** Escape raw text to a safe LLVM IR string literal form. */
    static std::string escapeForIR(const std::string& raw);
    /** Find a Line* by BASIC line number (nullptr if absent). */
    const Line* findLine(int line) const;
    /** Allocate a stack slot for a variable if not already allocated. */
    void ensureVarAllocated(std::ostringstream& out, const std::string& name);
    void ensureArrayAllocated(std::ostringstream& out, const std::string& name, int length);
    void ensureStringArrayAllocated(std::ostringstream& out, const std::string& name, int length);
    /** Sanitize BASIC variable name into a valid local IR identifier */
    static std::string sanitizeLocal(const std::string& name);
    /** Emit casts+store to assign a computed double to a typed variable */
    void storeNumberToVar(std::ostringstream& out, const std::string& name, const std::string& doubleValSSA);
    /** Reset a variable to zero/null according to its type */
    void resetVar(std::ostringstream& out, const std::string& name);
    /** Lookup numeric kind for variable (assumes non-string); defaults to Single */
    NumKind numKindOf(const std::string& name) const;
    // Lookup current inline binding for a variable name (if any)
    bool lookupBinding(const std::string& name, std::string& out) const;
    // Helper: determine if a variable name is string-typed
    bool isStringVarNameCG(const std::string& name) const;
    bool isStringArrayNameCG(const std::string& name) const;

    // Array helpers
    // Determine LLVM element type string for a numeric array name
    std::string arrayElemType(const std::string& name) const;
    // Emit a typed store into a numeric array element given the RHS as double
    void storeNumberToArrayElem(std::ostringstream& out,
                                const std::string& arrayName,
                                const std::string& elemPtrSSA,
                                const std::string& doubleValSSA);
    // Load a numeric array element as a double SSA value
    std::string loadArrayElemAsDouble(std::ostringstream& out,
                                      const std::string& arrayName,
                                      const std::string& elemPtrSSA);

    // cdGatherLinesAndDeletes split helpers
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    static void cdCollectLinesAndBounds(const Program& program,
                                        std::vector<int>& linesOut,
                                        std::map<int, const Line*>& lineMapOut,
                                        int& globalMin,
                                        int& globalMax);
    static void cdCollectDeleteAndOptions(const Program& program,
                                          bool& printZones,
                                          std::vector<std::pair<int,int>>& deleteRanges,
                                          int globalMin,
                                          int globalMax); // NOLINT(bugprone-easily-swappable-parameters)
    static void cdNormalizeBoundsAndRanges(int& globalMin,
                                           int& globalMax,
                                           std::vector<std::pair<int,int>>& deleteRanges);

    // Logging utilities
    /** Stream accessor: codegen-phase logger (ostream sink when disabled). */
    std::ostream& log();
    /** Stream accessor: semantics-phase logger (ostream sink when disabled). */
    std::ostream& logSem();
    /** Stream accessor: syntax-phase logger (unused here; provided for interface parity). */
    std::ostream& syntax();
    /** Human-readable name for a Stmt node kind (for logging). */
    static const char* nodeName(const Stmt* stmt);
    /** Human-readable name for an Expr node kind (for logging). */
    static const char* nodeName(const Expr* expr);

public:
    /***
     * Function: CodeGenerator::setLogPath
     * Purpose:
     *  - Enable code generation logging to the specified file path.
     * Inputs:
     *  - path: Destination file path for codegen logs
     */
    void setLogPath(const std::string& path);
    /***
     * Function: CodeGenerator::setSemanticLogPath
     * Purpose:
     *  - Enable semantic analysis logging to the specified file path.
     * Inputs:
     *  - path: Destination file path for semantic logs
     */
    void setSemanticLogPath(const std::string& path);
    // Control SQRT alias (non-standard). When false, only SQR is recognized.
    void setAllowSqrtAlias(bool allow);
private:
    bool allowSqrtAlias_{true};

    // -- Helpers to simplify emitFor() --
    /** Load a scalar variable as double for math/comparisons. */
    std::string loadVarAsDouble(std::ostringstream& out, const std::string& varName);

    /** Compute FOR loop condition as i1 given cur, end, step (inclusive). */
    std::string computeForCond(std::ostringstream& out,
                               const std::string& curVal,
                               const std::string& endReg,
                               const std::string& stepReg);

    /** Emit step increment and branch back to cond label. */
    void emitForIncrement(std::ostringstream& out,
                          const std::string& varName,
                          std::string_view stepReg,
                          const std::string& condLbl);

    /** Emit all statements inside a FOR body; returns true if body terminated (e.g., via GOTO). */
    bool emitForBodyStatements(std::ostringstream& out,
                               const ForStmt* forStmt,
                               const std::string& currLineLabel,
                               int& localCounter);
    bool emitForBodyStatement(std::ostringstream& out,
                              const Stmt* stmt,
                              const std::string& currLineLabel,
                              int& localCounter);

    // FOR body per-kind helpers (one function per file)
    void emitForHandleAssign(std::ostringstream& out, const AssignStmt* asg, const std::string& currLineLabel);
    void emitForHandlePrint(std::ostringstream& out, const PrintStmt* pr, const std::string& currLineLabel, int& localCounter);
    void emitForHandleMidAssign(std::ostringstream& out, const MidAssignStmt* mid, const std::string& currLineLabel, int& localCounter);
    void emitForHandleOnGoto(std::ostringstream& out, const OnGotoStmt* og, const std::string& currLineLabel, int& localCounter);
    void emitForHandleOnGosub(std::ostringstream& out, const OnGosubStmt* ogs, const std::string& currLineLabel, int& localCounter);
    bool emitForHandleGoto(std::ostringstream& out, const GotoStmt* gt);
    void emitForHandleGosub(std::ostringstream& out, const GosubStmt* gosubStmt, const std::string& currLineLabel, int& localCounter);
    void emitForHandleArrayAssign(std::ostringstream& out, const ArrayAssignStmt* aaset, const std::string& currLineLabel, int& localCounter);
    void emitForHandleStop(std::ostringstream& out);
    static void emitForHandleSystem(std::ostringstream& out);

    // -- Subroutine (GOSUB) per-kind helpers --
    void emitSubHandleAssign(std::ostringstream& out, const AssignStmt* asg, std::string_view entryLabel);
    void emitSubHandleMidAssign(std::ostringstream& out, const MidAssignStmt* mid, std::string_view entryLabel, int& localCounter);
    void emitSubHandlePrint(std::ostringstream& out, const struct PrintStmt* pr, std::string_view entryLabel, int& localCounter);
    void emitSubHandleInput(std::ostringstream& out, const struct InputStmt* ins, std::string_view entryLabel);
    void emitSubHandleIf(std::ostringstream& out, const struct IfStmt* ifStmt, std::string_view entryLabel, int& localCounter);
    void emitSubHandleGosub(std::ostringstream& out, const struct GosubStmt* gosubStmt, std::string_view entryLabel, int& localCounter);
    void emitSubHandleFor(std::ostringstream& out, const struct ForStmt* fs, std::string_view entryLabel, int& localCounter);

    // PRINT sub-helpers (FOR body)
    void emitForPrintPadZone(std::ostringstream& out, const PrintStmt* pr);
    void emitForPrintStringItem(std::ostringstream& out, const PrintStmt* pr, const StringExpr* se, bool addNL);
    void emitForPrintConstNumberItem(std::ostringstream& out, const PrintStmt* pr, double cv, bool addNL, bool nextStartsWithSpace);
    void emitForPrintDynamicOverride(std::ostringstream& out, const PrintStmt* printStmt, const std::string& val,
                                     std::string_view currLineLabel, int& localCounter);
    void emitForPrintDynamicAuto(std::ostringstream& out, const PrintStmt* pr, const std::string& val,
                                 bool addNL, bool nextStartsWithSpace,
                                 std::string_view currLineLabel, int& localCounter);

    /** Emit common error-path stores and handler dispatch switch. */
    void emitErrorDispatch(std::ostringstream& out, int errCode, int lineNo, int stmtIndex);

    /** Compute linearized index for multi-dim array indices (1-based optionBase_). */
    std::string emitLinearIndex(std::ostringstream& out,
                                const std::vector<std::string>& idxI64s,
                                const std::vector<int>& dims);
};

} // namespace gwbasic
