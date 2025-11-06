// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <map>
#include <ranges>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
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
        semVariables_ = r.variables;
        semStrings_ = r.stringLiterals;
        semLineNumbers_ = r.lineNumbers;
        semCommonVariables_ = r.commonVariables;
        arraySizes_ = r.arrays;
        userFunctions_ = r.userFunctions;
        semStringVariables_ = r.stringVariables;
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
    std::set<std::string> variables_;
    /*
     * Property: varAllocaName_
     * Purpose:
     *  - Map from variable name to its LLVM alloca symbol name.
     */
    std::map<std::string, std::string> varAllocaName_;
    /*
     * Property: strLiteralId_
     * Purpose:
     *  - Map string literal value to a unique id used for global names.
     */
    std::map<std::string, int> strLiteralId_;
    std::map<std::string, int> arraySizes_{};
    std::map<std::string, std::string> arrayAllocaName_{};
    // User-defined functions by uppercase name
    std::map<std::string, const DefFnStmt*> userFunctions_{};
    // Variables determined as string-typed (by suffix or DEFSTR)
    std::set<std::string> semStringVariables_{};
    // Variables numeric kind mapping (non-strings only)
    enum class NumKind { Int16, Long32, Single, Double };
    std::map<std::string, NumKind> semNumericKinds_{};
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
    std::set<std::string> semVariables_{};
    /*
     * Property: semStrings_
     * Purpose:
     *  - String literals set provided by semantics.
     */
    std::set<std::string> semStrings_{};
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
    std::set<std::string> semCommonVariables_{};
    /*
     * Property: commonVariables_
     * Purpose:
     *  - Variables declared as COMMON in the current program.
     */
    std::set<std::string> commonVariables_{};
    /*
     * Property: commonBeforeLine_
     * Purpose:
     *  - Snapshot of COMMON variables in effect before each line number to
     *    drive CHAIN scoping behavior.
     */
    std::map<int, std::set<std::string>> commonBeforeLine_{};
    // Snapshot of variables seen before each line (in source order)
    std::map<int, std::set<std::string>> varsBeforeLine_{};
    // Snapshot of arrays seen (DIM'd or referenced) before each line
    std::map<int, std::set<std::string>> arraysBeforeLine_{};
    // For error handlers: map trap start line -> first non-handler line after the
    // handler region (i.e., the line following the first line containing RESUME)
    std::map<int, int> handlerSkipAfter_{};
    // Mapping from 1000-based line region base (e.g., 0, 1000, 2000, ...)
    // to the DATA table index at the start of that region. Used to reset
    // the DATA pointer on CHAIN to a new program segment.
    std::map<int, int> regionDataStartIdx_{};
    // DATA items as string literal ids in program order
    std::vector<int> dataLiteralIds_{};

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
    void collectVarsForBeforeLineFromExpr(const Expr* e, std::set<std::string>& vars, std::set<std::string>& arrays);
    void collectVarsForBeforeLineFromStmt(const Stmt* s, std::set<std::string>& vars, std::set<std::string>& arrays);
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
};

} // namespace gwbasic
