// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <set>
#include <map>
#include <string>
#include <unordered_set>
#include <optional>
#include <vector>
#include <fstream>
#include "logger/Logger.h"

#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/semantics/SemanticError.h"
#include "basic_compiler/ast/DefFnStmt.h"
#include "basic_compiler/ast/DefTypeStmt.h"


namespace gwbasic {

/**
 * Class: SemanticAnalyzer
 *  - Performs a semantic pass over the AST to resolve symbols, record
 *    declarations/references, check simple structural constraints, and
 *    optionally emit a semantic phase log.
 */
class SemanticAnalyzer {
public:
    /*
     * Struct: SemanticAnalyzer::Result
     * Purpose:
     *  - Aggregate outputs from the semantic analysis pass.
     * Fields:
     *  - variables: Set of declared/implicitly-declared variable names
     *  - stringLiterals: Set of unique string literal values discovered
     *  - lineNumbers: Set of all program line numbers
     *  - commonVariables: Set of variables marked via COMMON
     */
    struct Result {
        std::set<std::string> variables;
        std::set<std::string> stringLiterals;
        std::set<int> lineNumbers;
        std::set<std::string> commonVariables;
        std::map<std::string,int> arrays; // 1-D arrays name->length
        // User-defined functions keyed by uppercase function name (e.g., "FNSQ")
        std::map<std::string, const DefFnStmt*> userFunctions;
        // Variables determined to be strings (by suffix or DEFSTR)
        std::set<std::string> stringVariables;
    };

    SemanticAnalyzer() = default;

    /*
     * Function: SemanticAnalyzer::setLogPath
     * Purpose:
     *  - Enable semantic-phase logging to the specified file path.
     * Inputs:
     *  - path: Filesystem path to write log entries
     * Outputs:
     *  - void (opens/truncates file; toggles logEnabled_)
     */
    void setLogPath(const std::string& path) {
        logger_.open(path, /*append=*/false);
        logger_.setEnabled(true);
    }

    /*
     * Function: SemanticAnalyzer::setStrictControlFlow
     * Purpose:
     *  - Configure whether missing control-flow targets cause errors or warnings.
     * Inputs:
     *  - strict: true to raise errors; false to log warnings only
     * Outputs:
     *  - void (sets internal flag)
     */
    void setStrictControlFlow(bool strict) { strictControlFlow_ = strict; }

    /*
     * Function: SemanticAnalyzer::analyze
     * Purpose:
     *  - Perform semantic analysis over a parsed Program AST.
     * Inputs:
     *  - program: Root AST to analyze
     * Outputs:
     *  - Result: Aggregated sets of variables, strings, lines, and COMMONs
     */
    Result analyze(const Program& program);

private:
    // Simple lexical scope stack. BASIC is global but supports nesting for future use.
    /*
     * Property: scopes_
     * Purpose:
     *  - Stack of lexical scopes containing declared variable names.
     * Notes:
     *  - Always contains at least the global scope at index 0.
     */
    std::vector<std::unordered_set<std::string>> scopes_{{}};

    /*
     * Property: vars_
     * Purpose:
     *  - Accumulated set of all variable names discovered.
     */
    std::set<std::string> vars_;

    /*
     * Property: strings_
     * Purpose:
     *  - Set of unique string literal values encountered.
     */
    std::set<std::string> strings_;

    /*
     * Property: lines_
     * Purpose:
     *  - Set of all program line numbers.
     */
    std::set<int> lines_;

    /*
     * Property: common_
     * Purpose:
     *  - Set of variables appearing in COMMON declarations.
     */
    std::set<std::string> common_;
    std::map<std::string,int> arrays_;
    // User-defined functions by uppercase name
    std::map<std::string, const DefFnStmt*> userFunctions_;
    // Current DEF FN parameter name (skip global reference tracking when set)
    std::optional<std::string> currentFnParam_{};
    // Default type mapping by letter (A..Z). Only String affects codegen typing.
    enum class DefaultKind { None, Int, Sng, Dbl, Str };
    DefaultKind defaultKinds_[26]{}; // initialized to None
    // Helper: determine if a variable name is string-typed by suffix or DEFSTR rules.
    bool varNameIsString(const std::string& name) const;

    // Logging via ostream-based logger
    logger::Logger logger_{};

    /*
     * Property: currentLine_
     * Purpose:
     *  - Current line number for contextual logging.
     */
    int currentLine_{0};

    /*
     * Property: strictControlFlow_
     * Purpose:
     *  - When true, missing control-flow targets raise errors; otherwise warnings.
     */
    bool strictControlFlow_{true};

    /**
     * Function: SemanticAnalyzer::enterScope
     * Purpose:
     *  - Push a new lexical scope onto the stack and log the event.
     */
    void enterScope() { scopes_.emplace_back(); log() << "ScopeEnter" << '\n'; }

    /**
     * Function: SemanticAnalyzer::exitScope
     * Purpose:
     *  - Pop the current scope if not global and log the event.
     */
    void exitScope() { if (scopes_.size() > 1) scopes_.pop_back(); log() << "ScopeExit" << '\n'; }

    /**
     * Function: SemanticAnalyzer::isDeclared
     * Purpose:
     *  - Check whether a variable name exists in any active scope.
     * Inputs:
     *  - name: Variable identifier
     * Outputs:
     *  - bool: true if declared in an active scope
     */
    bool isDeclared(const std::string& name) const;

    /**
     * Function: SemanticAnalyzer::declare
     * Purpose:
     *  - Declare a variable in the global scope and record it.
     * Inputs:
     *  - name: Variable identifier
     */
    void declare(const std::string& name);

    /**
     * Function: SemanticAnalyzer::reference
     * Purpose:
     *  - Reference a variable; implicitly declares if undeclared and logs.
     * Inputs:
     *  - name: Variable identifier
     *  - pos: Source position for logging context
     */
    void reference(const std::string& name, const SourcePos& pos);

    // Stream accessor for integration symmetry with other phases
    std::ostream& log() { return logger_.stream(); }

    /**
     * Function: SemanticAnalyzer::analyzeLine
     * Purpose:
     *  - Analyze a single program line and its statements.
     * Inputs:
     *  - line: Line node with statements to analyze
     */
    void analyzeLine(const Line& line);

    /**
     * Function: SemanticAnalyzer::analyzeStmt
     * Purpose:
     *  - Analyze a statement node, updating state or throwing on errors.
     */
    void analyzeStmt(const Stmt* s);

    /**
     * Function: SemanticAnalyzer::analyzeExpr
     * Purpose:
     *  - Analyze an expression node for type/usage and collect strings.
     */
    void analyzeExpr(const Expr* e);

    // Basic type analysis (numeric or string)
    /**
     * Enum: SemanticAnalyzer::ValueType
     * Purpose:
     *  - Classify expressions as numeric or string for type checking.
     * Values:
     *  - Number, String
     */
    enum class ValueType { Number, String };

    /**
     * Function: SemanticAnalyzer::typeOf
     * Purpose:
     *  - Determine the value type of an expression, raising errors for
     *    invalid operations (e.g., string arithmetic) as needed.
     */
    ValueType typeOf(const Expr* e);

    // Extra semantic helpers
    /**
     * Function: SemanticAnalyzer::isComparisonExpr
     * Purpose:
     *  - Identify whether an expression is a comparison operation.
     */
    static bool isComparisonExpr(const Expr* e);

    /**
     * Function: SemanticAnalyzer::constEval
     * Purpose:
     *  - Attempt constant-fold evaluation of an expression.
     * Inputs:
     *  - e: Expression to evaluate
     *  - out: Reference to receive numeric result
     * Outputs:
     *  - bool: true if constant value was produced
     */
    static bool constEval(const Expr* e, double& out);

    // Function utilities
    /**
     * Function: SemanticAnalyzer::isKnownNumericFunction
     * Purpose:
     *  - Determine if a function name is a supported numeric intrinsic.
     */
    static bool isKnownNumericFunction(const std::string& name);
    /**
     * Function: SemanticAnalyzer::isKnownStringFunction
     * Purpose:
     *  - Determine if a function name is a supported string intrinsic.
     */
    static bool isKnownStringFunction(const std::string& name);

    /**
     * Function: SemanticAnalyzer::expectedArity
     * Purpose:
     *  - Return the expected argument count for a given intrinsic name.
     */
    static int expectedArity(const std::string& name);
};

} // namespace gwbasic
