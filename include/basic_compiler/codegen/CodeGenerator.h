// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "basic_compiler/ast/Program.h"
#include "basic_compiler/codegen/CodeGenError.h"

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

    // Emission helpers
    void emitHeader(std::ostringstream& out);
    void emitGlobals(std::ostringstream& out);
    void emitMainPrologue(std::ostringstream& out);
    void emitMainEpilogue(std::ostringstream& out);
    void emitLineBlock(std::ostringstream& out, const Line& line, int lineIndex, int lastIndex);
    void emitFor(std::ostringstream& out, const ForStmt* fs, const std::string& currLineLabel, int& localCounter);
    void emitSubroutineInline(std::ostringstream& out, int targetLine, const std::string& entryLabel, const std::string& returnLabel);

    // Expression lowering
    std::string emitExpr(std::ostringstream& out, const Expr* e, const std::string& currBlockSuffix);
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
    static const char* nodeName(const Stmt* s) {
        if (dynamic_cast<const AssignStmt*>(s)) return "AssignStmt";
        if (dynamic_cast<const PrintStmt*>(s)) return "PrintStmt";
        if (dynamic_cast<const GotoStmt*>(s)) return "GotoStmt";
        if (dynamic_cast<const GosubStmt*>(s)) return "GosubStmt";
        if (dynamic_cast<const ReturnStmt*>(s)) return "ReturnStmt";
        if (dynamic_cast<const IfStmt*>(s)) return "IfStmt";
        if (dynamic_cast<const InputStmt*>(s)) return "InputStmt";
        if (dynamic_cast<const ForStmt*>(s)) return "ForStmt";
        if (dynamic_cast<const EndStmt*>(s)) return "EndStmt";
        return "Stmt";
    }
    static const char* nodeName(const Expr* e) {
        if (dynamic_cast<const NumberExpr*>(e)) return "NumberExpr";
        if (dynamic_cast<const StringExpr*>(e)) return "StringExpr";
        if (dynamic_cast<const VarExpr*>(e)) return "VarExpr";
        if (dynamic_cast<const UnaryExpr*>(e)) return "UnaryExpr";
        if (dynamic_cast<const BinaryExpr*>(e)) return "BinaryExpr";
        return "Expr";
    }

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
