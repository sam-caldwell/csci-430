// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>

#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/semantics/SemanticError.h"

namespace gwbasic {

/**
 * Class: SemanticAnalyzer
 *  - Performs a semantic pass over the AST to resolve symbols, record
 *    declarations/references, check simple structural constraints, and
 *    optionally emit a semantic phase log.
 */
class SemanticAnalyzer {
public:
    struct Result {
        std::set<std::string> variables;
        std::set<std::string> stringLiterals;
        std::set<int> lineNumbers;
    };

    SemanticAnalyzer() = default;

    void setLogPath(const std::string& path) {
        if (log_.is_open()) log_.close();
        log_.open(path, std::ios::out | std::ios::trunc);
        logEnabled_ = log_.is_open();
    }

    void setStrictControlFlow(bool strict) { strictControlFlow_ = strict; }

    Result analyze(const Program& program);

private:
    // Simple lexical scope stack. BASIC is global, but support nesting for future use.
    std::vector<std::unordered_set<std::string>> scopes_{{}}; // start with global scope
    std::set<std::string> vars_;
    std::set<std::string> strings_;
    std::set<int> lines_;
    bool logEnabled_{false};
    std::ofstream log_;
    int currentLine_{0};
    bool strictControlFlow_{true};

    void enterScope() { scopes_.emplace_back(); log("ScopeEnter"); }
    void exitScope() { if (scopes_.size() > 1) scopes_.pop_back(); log("ScopeExit"); }

    bool isDeclared(const std::string& name) const;
    void declare(const std::string& name);
    void reference(const std::string& name, const SourcePos& pos);
    void log(const std::string& msg) { if (logEnabled_ && log_.is_open()) log_ << msg << '\n'; }

    void analyzeLine(const Line& line);
    void analyzeStmt(const Stmt* s);
    void analyzeExpr(const Expr* e);

    // Basic type analysis (numeric or string)
    enum class ValueType { Number, String };
    ValueType typeOf(const Expr* e);
};

} // namespace gwbasic
