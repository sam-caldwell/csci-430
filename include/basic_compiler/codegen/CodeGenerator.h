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
    void setSemantics(const SemanticAnalyzer::Result& r) {
        semProvided_ = true;
        semVariables_.clear();
        semVariables_.insert(r.variables.begin(), r.variables.end());
        semStrings_.clear();
        semStrings_.insert(r.stringLiterals.begin(), r.stringLiterals.end());
        semLineNumbers_ = r.lineNumbers;
        semCommonVariables_.clear();
        semCommonVariables_.insert(r.commonVariables.begin(), r.commonVariables.end());
        arrayDims_.clear();
        arrayDims_.insert(r.arrays.begin(), r.arrays.end());
        optionBase_ = r.optionBase;
        printZones_ = r.printZones;
        userFunctions_.clear();
        userFunctions_.insert(r.userFunctions.begin(), r.userFunctions.end());
        semStringVariables_.clear();
        semStringVariables_.insert(r.stringVariables.begin(), r.stringVariables.end());
        // Map numeric kinds from semantics into codegen's representation
        semNumericKinds_.clear();
        for (const auto& [name, kind] : r.numericKinds) {
            switch (kind) {
                case SemanticAnalyzer::Result::NumericKind::Int16: semNumericKinds_[name] = NumKind::Int16; break;
                case SemanticAnalyzer::Result::NumericKind::Long32: semNumericKinds_[name] = NumKind::Long32; break;
                case SemanticAnalyzer::Result::NumericKind::Single: semNumericKinds_[name] = NumKind::Single; break;
                case SemanticAnalyzer::Result::NumericKind::Double: semNumericKinds_[name] = NumKind::Double; break;
            }
        }
    }

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
    /*
     * Property: lineNumbers_
     * Purpose:
     *  - Sorted list of all program line numbers (emission order driver).
     */
    std::vector<int> lineNumbers_;
    /*
     * Property: lineMap_
     * Purpose:
     *  - Map from line number to Line* for quick lookup during emission.
     */
    std::map<int, const Line*> lineMap_;
    /*
     * Property: currentLine_
     * Purpose:
     *  - Currently emitting line number for logging context.
     */
    int currentLine_{0};
    /*
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
    /*
     * Property: semProvided_
     * Purpose:
     *  - Whether setSemantics() was called to seed generator state.
     */
    bool semProvided_{false};
    /*
     * Property: semVariables_
     * Purpose:
     *  - Variables set provided by semantic analysis.
     */
    std::set<std::string, std::less<>> semVariables_{};
    /*
     * Property: semStrings_
     * Purpose:
     *  - String literals set provided by semantics.
     */
    std::set<std::string, std::less<>> semStrings_{};
    /*
     * Property: semLineNumbers_
     * Purpose:
     *  - Line numbers set provided by semantics.
     */
    std::set<int> semLineNumbers_{};
    /*
     * Property: semCommonVariables_
     * Purpose:
     *  - Variables marked as COMMON by semantics.
     */
    std::set<std::string, std::less<>> semCommonVariables_{};
    /*
     * Property: commonVariables_
     * Purpose:
     *  - Variables declared as COMMON in the current program.
     */
    std::set<std::string, std::less<>> commonVariables_{};
    /*
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
    std::string nextTemp() { std::string s = "%t"; s += std::to_string(++tempCounter_); return s; }
    /**
     * Function: CodeGenerator::globalStringName
     * Purpose:
     *  - Derive a stable global symbol name for a string literal id.
     * Inputs:
     *  - id: Unique integer for the string literal
     * Outputs:
     *  - std::string: Global symbol (e.g., "@.str.5")
     */
    static std::string globalStringName(int id) { std::string s = "@.str."; s += std::to_string(id); return s; }
    /**
     * Function: CodeGenerator::lineLabelName
     * Purpose:
     *  - Build a canonical basic block label for a BASIC line number.
     * Inputs:
     *  - ln: BASIC source line number
     * Outputs:
     *  - std::string: Label (e.g., "line100")
     */
    static std::string lineLabelName(int ln) { std::string s = "line"; s += std::to_string(ln); return s; }
    /** Label for re-executing a specific statement index within a line. 1-based index. */
    static std::string resumeLabelName(int ln, int stmtIndex) {
        std::string s = "resume_l"; s += std::to_string(ln); s += "_"; s += std::to_string(stmtIndex); return s;
    }
    /** Label for resuming at the statement after a given index within a line. 1-based index. */
    static std::string resumeNextLabelName(int ln, int stmtIndex) {
        std::string s = "resume_next_l"; s += std::to_string(ln); s += "_"; s += std::to_string(stmtIndex); return s;
    }

    // Declaration collection
    /** Collect declarations, variables, strings, and line ordering. */
    void collectDecls(const Program& program);
    // Helpers to collect variable/array references for varsBeforeLine_/arraysBeforeLine_
    void collectVarsForBeforeLineFromExpr(const Expr* e, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    void collectVarsForBeforeLineFromStmt(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);

    // Per-kind handlers to reduce complexity; implemented one-per-file
    bool handleAssignBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleArrayAssignBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleIfBlockBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleIfBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleForBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleWhileBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handlePrintBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleInputBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleReadBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleDimBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& arrays);
    bool handleSwapBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleEraseBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& arrays);
    bool handleWriteBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleOnGotoBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    bool handleOnGosubBeforeLine(const Stmt* s, std::set<std::string, std::less<>>& vars, std::set<std::string, std::less<>>& arrays);
    /** Collect variables/strings referenced by an expression. */
    void collectExprVars(const Expr* e);
    /** Collect variables/strings/COMMON from a statement (recursive). */
    void collectStmtVars(const Stmt* s);
    // Lightweight scan for RND usage independent of semantics
    /** Scan expression for RND() usage to enable helper emission. */
    void scanExprForRnd(const Expr* e);
    /** Scan statement (and children) for RND() usage. */
    void scanStmtForRnd(const Stmt* s);
    /** Scan statement (and children) for STOP usage. */
    void scanStmtForStop(const Stmt* s);

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

    // Expression lowering
    /** Lower an expression to SSA value; returns its name. */
    std::string emitExpr(std::ostringstream& out, const Expr* e, [[maybe_unused]] const std::string& currBlockSuffix);
    // Helper: determine whether an expression is string-typed (for codegen routing)
    bool isStringExpr(const Expr* e) const;
    /** Lower a comparison expression to an i1 predicate value. */
    std::string emitComparison(std::ostringstream& out, const BinaryExpr* c);

    // Utilities
    /** Escape raw text to a safe LLVM IR string literal form. */
    static std::string escapeForIR(const std::string& s);
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
    NumKind numKindOf(const std::string& name) const {
        auto it = semNumericKinds_.find(name);
        if (it != semNumericKinds_.end()) return it->second;
        return NumKind::Single;
    }
    // Lookup current inline binding for a variable name (if any)
    bool lookupBinding(const std::string& name, std::string& out) const {
        for (const auto & it : std::ranges::reverse_view(bindingStack_)) {
            auto f = it.find(name);
            if (f != it.end()) { out = f->second; return true; }
        }
        return false;
    }
    // Helper: determine if a variable name is string-typed
    bool isStringVarNameCG(const std::string& name) const {
        if (!name.empty() && name.back() == Symbols::DOLLARSIGN.first()) return true;
        return semStringVariables_.contains(name);
    }
    bool isStringArrayNameCG(const std::string& name) const { return isStringVarNameCG(name); }

    // Array helpers
    // Determine LLVM element type string for a numeric array name
    std::string arrayElemType(const std::string& name) const {
        switch (numKindOf(name)) {
            case NumKind::Int16: return "i32";   // Integer arrays map to i32
            case NumKind::Long32: return "i64";  // Long arrays map to i64
            case NumKind::Single: return "float"; // Single arrays are true float
            case NumKind::Double: default: return "double";
        }
    }
    // Emit a typed store into a numeric array element given the RHS as double
    void storeNumberToArrayElem(std::ostringstream& out,
                                const std::string& arrayName,
                                const std::string& elemPtrSSA,
                                const std::string& doubleValSSA) {
        switch (numKindOf(arrayName)) {
            case NumKind::Int16: {
                std::string cvt = nextTemp();
                { std::string ir = std::format("  {} = fptosi double {} to i32", cvt, doubleValSSA); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i32 {}, ptr {}", cvt, elemPtrSSA); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Long32: {
                std::string cvt = nextTemp();
                { std::string ir = std::format("  {} = fptosi double {} to i64", cvt, doubleValSSA); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store i64 {}, ptr {}", cvt, elemPtrSSA); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Single: {
                std::string cvt = nextTemp();
                { std::string ir = std::format("  {} = fptrunc double {} to float", cvt, doubleValSSA); out << ir << Symbols::LF; }
                { std::string ir = std::format("  store float {}, ptr {}", cvt, elemPtrSSA); out << ir << Symbols::LF; }
                break;
            }
            case NumKind::Double: {
                { std::string ir = std::format("  store double {}, ptr {}", doubleValSSA, elemPtrSSA); out << ir << Symbols::LF; }
                break;
            }
        }
    }
    // Load a numeric array element as a double SSA value
    std::string loadArrayElemAsDouble(std::ostringstream& out,
                                      const std::string& arrayName,
                                      const std::string& elemPtrSSA) {
        switch (numKindOf(arrayName)) {
            case NumKind::Int16: {
                std::string v = nextTemp(); { std::string ir = std::format("  {} = load i32, ptr {}", v, elemPtrSSA); out << ir << Symbols::LF; }
                std::string d = nextTemp(); { std::string ir = std::format("  {} = sitofp i32 {} to double", d, v); out << ir << Symbols::LF; }
                return d;
            }
            case NumKind::Long32: {
                std::string v = nextTemp(); { std::string ir = std::format("  {} = load i64, ptr {}", v, elemPtrSSA); out << ir << Symbols::LF; }
                std::string d = nextTemp(); { std::string ir = std::format("  {} = sitofp i64 {} to double", d, v); out << ir << Symbols::LF; }
                return d;
            }
            case NumKind::Single: {
                std::string v = nextTemp(); { std::string ir = std::format("  {} = load float, ptr {}", v, elemPtrSSA); out << ir << Symbols::LF; }
                std::string d = nextTemp(); { std::string ir = std::format("  {} = fpext float {} to double", d, v); out << ir << Symbols::LF; }
                return d;
            }
            case NumKind::Double: default: {
                std::string d = nextTemp(); { std::string ir = std::format("  {} = load double, ptr {}", d, elemPtrSSA); out << ir << Symbols::LF; }
                return d;
            }
        }
    }

    // Logging utilities
    /** Stream accessor: codegen-phase logger (ostream sink when disabled). */
    std::ostream& log() { return codegenLogger_.stream(); }
    /** Stream accessor: semantics-phase logger (ostream sink when disabled). */
    std::ostream& logSem() { return semLogger_.stream(); }
    /** Stream accessor: syntax-phase logger (unused here; provided for interface parity). */
    std::ostream& syntax() { return syntaxLogger_.stream(); }
    /** Human-readable name for a Stmt node kind (for logging). */
    static const char* nodeName(const Stmt* s) { return prettyName(s ? s->getKind() : NodeKind::AbstractStmt); }
    /** Human-readable name for an Expr node kind (for logging). */
    static const char* nodeName(const Expr* e) { return prettyName(e ? e->getKind() : NodeKind::AbstractExpr); }

public:
    /**
     * Function: CodeGenerator::setLogPath
     * Purpose:
     *  - Enable code generation logging to the specified file path.
     * Inputs:
     *  - path: Destination file path for codegen logs
     */
    void setLogPath(const std::string& path) {
        codegenLogger_.open(path, /*append=*/false);
        codegenLogger_.setEnabled(true);
    }
    /**
     * Function: CodeGenerator::setSemanticLogPath
     * Purpose:
     *  - Enable semantic analysis logging to the specified file path.
     * Inputs:
     *  - path: Destination file path for semantic logs
     */
    void setSemanticLogPath(const std::string& path) {
        semLogger_.open(path, /*append=*/false);
        semLogger_.setEnabled(true);
    }
    // Control SQRT alias (non-standard). When false, only SQR is recognized.
    void setAllowSqrtAlias(bool allow) { allowSqrtAlias_ = allow; }
private:
    bool allowSqrtAlias_{true};

    // -- Helpers to simplify emitFor() --
    /** Load a scalar variable as double for math/comparisons. */
    std::string loadVarAsDouble(std::ostringstream& out, const std::string& varName) {
        switch (numKindOf(varName)) {
            case NumKind::Int16: {
                std::string l = nextTemp();
                out << std::format("  {} = load i16, ptr {}", l, varAllocaName_[varName]) << Symbols::LF;
                std::string d = nextTemp();
                out << std::format("  {} = sitofp i16 {} to double", d, l) << Symbols::LF;
                return d;
            }
            case NumKind::Long32: {
                std::string l = nextTemp();
                out << std::format("  {} = load i32, ptr {}", l, varAllocaName_[varName]) << Symbols::LF;
                std::string d = nextTemp();
                out << std::format("  {} = sitofp i32 {} to double", d, l) << Symbols::LF;
                return d;
            }
            case NumKind::Single: {
                std::string l = nextTemp();
                out << std::format("  {} = load float, ptr {}", l, varAllocaName_[varName]) << Symbols::LF;
                std::string d = nextTemp();
                out << std::format("  {} = fpext float {} to double", d, l) << Symbols::LF;
                return d;
            }
            case NumKind::Double: default: {
                std::string d = nextTemp();
                out << std::format("  {} = load double, ptr {}", d, varAllocaName_[varName]) << Symbols::LF;
                return d;
            }
        }
    }

    /** Compute FOR loop condition as i1 given cur, end, step (inclusive). */
    std::string computeForCond(std::ostringstream& out,
                               const std::string& curVal,
                               const std::string& endReg,
                               const std::string& stepReg) {
        std::string isNeg = nextTemp();
        out << std::format("  {} = fcmp olt double {}, 0.0", isNeg, stepReg) << Symbols::LF;
        std::string condLe = nextTemp();
        out << std::format("  {} = fcmp ole double {}, {}", condLe, curVal, endReg) << Symbols::LF;
        std::string condGe = nextTemp();
        out << std::format("  {} = fcmp oge double {}, {}", condGe, curVal, endReg) << Symbols::LF;
        std::string cond = nextTemp();
        out << std::format("  {} = select i1 {}, i1 {}, i1 {}", cond, isNeg, condGe, condLe) << Symbols::LF;
        return cond;
    }

    /** Emit step increment and branch back to cond label. */
    void emitForIncrement(std::ostringstream& out,
                          const std::string& varName,
                          const std::string& stepReg,
                          const std::string& condLbl) {
        std::string vcur = loadVarAsDouble(out, varName);
        std::string vnext = nextTemp();
        out << std::format("  {} = fadd double {}, {}", vnext, vcur, stepReg) << Symbols::LF;
        storeNumberToVar(out, varName, vnext);
        out << std::format("  br label %{}", condLbl) << Symbols::LF;
    }

    /** Emit all statements inside a FOR body. Sets forTerminated when body ends with branch. */
    void emitForBodyStatements(std::ostringstream& out,
                               const ForStmt* fs,
                               const std::string& currLineLabel,
                               int& localCounter,
                               bool& forTerminated);

    /** Emit common error-path stores and handler dispatch switch. */
    void emitErrorDispatch(std::ostringstream& out, int errCode, int lineNo, int stmtIndex) {
        out << std::format("  store i32 {}, ptr @gwb_err_code", errCode) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_err_line", lineNo) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_resume_line", lineNo) << Symbols::LF;
        out << std::format("  store i32 {}, ptr @gwb_resume_stmt", stmtIndex) << Symbols::LF;
        out << std::format("  store i1 true, ptr @gwb_in_handler") << Symbols::LF;
        std::string trap = nextTemp();
        out << std::format("  {} = load i32, ptr @gwb_err_trap_line", trap) << Symbols::LF;
        out << std::format("  switch i32 {}, label %exit [", trap) << Symbols::LF;
        for (const auto & [lnum, lp] : lineMap_) {
            (void)lp;
            out << std::format("    i32 {}, label %{}", lnum, lineLabelName(lnum)) << Symbols::LF;
        }
        out << "  ]" << Symbols::LF;
    }

    /** Compute linearized index for multi-dim array indices (1-based optionBase_). */
    std::string emitLinearIndex(std::ostringstream& out,
                                const std::vector<std::string>& idxI64s,
                                const std::vector<int>& dims) {
        // Compute strides on host
        std::vector<long long> extents; extents.reserve(dims.size());
        for (size_t di = 0; di < dims.size(); ++di) {
            long long e = static_cast<long long>(dims[di]) - optionBase_ + 1; if (e < 0) e = 0; extents.push_back(e);
        }
        std::vector<long long> strides(dims.size(), 1);
        for (int di = static_cast<int>(dims.size()) - 2; di >= 0; --di) strides[di] = strides[di + 1] * extents[di + 1];
        // Adjust each index by base
        std::vector<std::string> adjs; adjs.reserve(idxI64s.size());
        for (const auto& ii : idxI64s) { std::string a = nextTemp(); out << std::format("  {} = sub i64 {}, {}", a, ii, optionBase_) << Symbols::LF; adjs.push_back(a); }
        // Multiply-accumulate
        std::string lin = nextTemp(); out << std::format("  {} = mul i64 {}, {}", lin, adjs[0], strides[0]) << Symbols::LF;
        for (size_t di = 1; di < adjs.size(); ++di) {
            std::string t = nextTemp(); out << std::format("  {} = mul i64 {}, {}", t, adjs[di], strides[di]) << Symbols::LF;
            std::string s2 = nextTemp(); out << std::format("  {} = add i64 {}, {}", s2, lin, t) << Symbols::LF; lin = s2;
        }
        return lin;
    }
};

} // namespace gwbasic
