// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/CallExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/AssignStmt.h"
#include "basic_compiler/ast/PrintStmt.h"
#include "basic_compiler/ast/InputStmt.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/GosubStmt.h"
#include "basic_compiler/ast/EndStmt.h"
#include "basic_compiler/ast/ReturnStmt.h"
#include "basic_compiler/ast/RandomizeStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/RunStmt.h"
#include "basic_compiler/ast/CommonStmt.h"
#include "basic_compiler/ast/ChainStmt.h"
#include "basic_compiler/ast/MergeStmt.h"
#include "basic_compiler/codegen/CodeGenError.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/ast/Traits.h"

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
    CodeGenerator() = default;

    /** Convert Program to LLVM IR (text form). */
    std::string generate(const Program& program);
    /** Provide precomputed semantic results (variables, strings, lines). */
    void setSemantics(const SemanticAnalyzer::Result& r) {
        semProvided_ = true;
        semVariables_ = r.variables;
        semStrings_ = r.stringLiterals;
        semLineNumbers_ = r.lineNumbers;
        semCommonVariables_ = r.commonVariables;
    }

private:
    // Counters and symbol maps
    int tempCounter_{0};
    int strCounter_{0};
    std::set<std::string> variables_;
    std::map<std::string, std::string> varAllocaName_;
    std::map<std::string, int> strLiteralId_; // string -> id
    std::vector<int> lineNumbers_; // sorted line numbers
    std::map<int, const Line*> lineMap_;
    int currentLine_{0};
    bool needsRndHelper_{false};
    // Optional semantic input
    bool semProvided_{false};
    std::set<std::string> semVariables_{};
    std::set<std::string> semStrings_{};
    std::set<int> semLineNumbers_{};
    std::set<std::string> semCommonVariables_{};
    std::set<std::string> commonVariables_{};

    // Phase logging
    bool logEnabled_{false};
    std::string logPath_{};
    std::ofstream logFile_;
    bool semLogEnabled_{false};
    std::string semLogPath_{};
    std::ofstream semLogFile_;

    // Naming helpers
    std::string nextTemp() { std::string s = "%t"; s += std::to_string(++tempCounter_); return s; }
    static std::string globalStringName(int id) { std::string s = "@.str."; s += std::to_string(id); return s; }
    static std::string lineLabelName(int ln) { std::string s = "line"; s += std::to_string(ln); return s; }

    // Declaration collection
    void collectDecls(const Program& program);
    void collectExprVars(const Expr* e);
    void collectStmtVars(const Stmt* s);
    // Lightweight scan for RND usage independent of semantics
    void scanExprForRnd(const Expr* e);
    void scanStmtForRnd(const Stmt* s);

    // Emission helpers
    void emitHeader(std::ostringstream& out);
    void emitGlobals(std::ostringstream& out);
    void emitMainPrologue(std::ostringstream& out);

    static void emitMainEpilogue(std::ostringstream& out);
    void emitLineBlock(std::ostringstream& out, const Line& line, int lineIndex, int lastIndex);
    void emitFor(std::ostringstream& out, const ForStmt* fs, const std::string& currLineLabel, int& localCounter);
    void emitIfBlock(std::ostringstream& out, const IfBlockStmt* ib, const std::string& currLineLabel, int& localCounter);
    void emitWhile(std::ostringstream& out, const WhileStmt* ws, const std::string& currLineLabel, int& localCounter);
    void emitSubroutineInline(std::ostringstream& out, int targetLine, const std::string& entryLabel, const std::string& returnLabel);

    // Expression lowering
    std::string emitExpr(std::ostringstream& out, const Expr* e, [[maybe_unused]] const std::string& currBlockSuffix);
    std::string emitComparison(std::ostringstream& out, const BinaryExpr* c);

    // Utilities
    static std::string escapeForIR(const std::string& s);
    const Line* findLine(int line) const;
    void ensureVarAllocated(std::ostringstream& out, const std::string& name);

    // Logging utilities
    void log(const std::string& msg) {
        if (logEnabled_ && logFile_.is_open()) logFile_ << msg << "\n";
    }
    void logSem(const std::string& msg) {
        if (semLogEnabled_ && semLogFile_.is_open()) semLogFile_ << msg << "\n";
    }
    static const char* nodeName(const Stmt* s) { return prettyName(s ? s->getKind() : NodeKind::AbstractStmt); }
    static const char* nodeName(const Expr* e) { return prettyName(e ? e->getKind() : NodeKind::AbstractExpr); }

public:
    /** Enable code generation logging to the specified file path. */
    void setLogPath(const std::string& path) {
        logPath_ = path;
        logEnabled_ = true;
        if (logFile_.is_open()) logFile_.close();
        logFile_.open(logPath_, std::ios::out | std::ios::trunc);
    }
    /** Enable semantic analysis logging to the specified file path. */
    void setSemanticLogPath(const std::string& path) {
        semLogPath_ = path;
        semLogEnabled_ = true;
        if (semLogFile_.is_open()) semLogFile_.close();
        semLogFile_.open(semLogPath_, std::ios::out | std::ios::trunc);
    }
};

} // namespace gwbasic
