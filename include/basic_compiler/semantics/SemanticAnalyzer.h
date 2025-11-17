// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include "basic_compiler/ast/DefFnStmt.h"
#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/SourcePos.h"
#include "basic_compiler/ast/Stmt.h"
#include "basic_compiler/semantics/SemanticError.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <streambuf>
#include <map>
#include <optional>
#include <ostream>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>


namespace gwbasic {

// Forward declarations to avoid heavy includes and mismatched tag warnings
struct UnaryExpr;
struct BinaryExpr;

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
        // Arrays with per-dimension upper bounds (inclusive)
        std::map<std::string, std::vector<int>> arrays;
        // OPTION BASE value (0 or 1)
        int optionBase{0};
        // OPTION PRINTZONES flag (false by default)
        bool printZones{false};
        // User-defined functions keyed by uppercase function name (e.g., "FNSQ")
        std::map<std::string, const DefFnStmt*> userFunctions;
        // Variables determined to be strings (by suffix or DEFSTR)
        std::set<std::string> stringVariables;
        // Per-variable numeric kind (for non-strings). Only includes non-string vars.
        enum class NumericKind : std::uint8_t { Int16, Long32, Single, Double };
        std::map<std::string, NumericKind> numericKinds;
    };

    SemanticAnalyzer() = default;

    // Logging removed; no configuration needed.

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

    // Control SQRT aliasing (non-standard extension). When false, only SQR is allowed.
    void setAllowSqrtAlias(bool allow) { allowSqrtAlias_ = allow; }

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
    std::map<std::string, std::vector<int>> arrays_;
    // Historical record of DIM'd arrays (last DIM wins) for codegen lengths
    std::map<std::string, std::vector<int>> allArrays_;
    int optionBase_{0};
    bool printZones_{false};
    // User-defined functions by uppercase name
    std::map<std::string, const DefFnStmt*> userFunctions_;
    // Current DEF FN parameter name (skip global reference tracking when set)
    std::optional<std::string> currentFnParam_;
    // Default type mapping by letter (A..Z). Only String affects codegen typing.
    enum class DefaultKind : std::uint8_t { None, Int, Sng, Dbl, Str };
    static constexpr std::size_t kAlphabetSize = 26; // NOLINT(readability-magic-numbers,cppcoreguidelines-avoid-magic-numbers)
    std::array<DefaultKind, kAlphabetSize> defaultKinds_{}; // initialized to None
    // Helper: determine if a variable name is string-typed by suffix or DEFSTR rules.
    bool varNameIsString(const std::string& name) const;
    // Helper: determine numeric kind for a non-string variable name
    Result::NumericKind numericKindOf(const std::string& name) const;

    // Logging removed; use a null sink

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
    bool allowSqrtAlias_{true};

    /**
     * Function: SemanticAnalyzer::enterScope
     * Purpose:
     *  - Push a new lexical scope onto the stack.
     */
    void enterScope() { scopes_.emplace_back(); }

    /**
     * Function: SemanticAnalyzer::exitScope
     * Purpose:
     *  - Pop the current scope if not global.
     */
    void exitScope() { if (scopes_.size() > 1) { scopes_.pop_back(); } }

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
    void reference(const std::string& name, const SourcePos& position);

    // Stream accessor: null sink for disabled logging
    std::ostream& log() {
        struct NullBuf : public std::streambuf { int overflow(int c) override { return traits_type::not_eof(c); } };
        static NullBuf nb;
        static std::ostream os(&nb);
        return os;
    }

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
    void analyzeStmt(const Stmt* stmt);

    /**
     * Function: SemanticAnalyzer::analyzeExpr
     * Purpose:
     *  - Analyze an expression node for type/usage and collect strings.
     */
    void analyzeExpr(const Expr* expr);

    // Basic type analysis (numeric or string)
    /**
     * Enum: SemanticAnalyzer::ValueType
     * Purpose:
     *  - Classify expressions as numeric or string for type checking.
     * Values:
     *  - Number, String
     */
    enum class ValueType : std::uint8_t { Number, String };

    /**
     * Function: SemanticAnalyzer::typeOf
     * Purpose:
     *  - Determine the value type of an expression, raising errors for
     *    invalid operations (e.g., string arithmetic) as needed.
     */
    ValueType typeOf(const Expr* expr);

    // Extra semantic helpers
    /**
     * Function: SemanticAnalyzer::isComparisonExpr
     * Purpose:
     *  - Identify whether an expression is a comparison operation.
     */
    static bool isComparisonExpr(const Expr* expr);

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
    static bool constEval(const Expr* expr, double& out);

    // Helpers for constEval (defined in separate TUs to maintain one-function-per-file)
    static bool constEvalUnary(const UnaryExpr& unary, double& out);
    static bool constEvalBinary(const BinaryExpr& binary, double& out);

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

    // Friend accessor to enable unit tests to query private arity table
    friend class SemanticAnalyzerArityAccessorForTests;
    // Friend accessor to enable unit tests to invoke private constEval
    friend class SemanticAnalyzerConstEvalAccessorForTests;
};

// Minimal friend accessor for tests: exposes expectedArity without widening API surface
class SemanticAnalyzerArityAccessorForTests {
public:
    static int expectedArity(const std::string& name) { return SemanticAnalyzer::expectedArity(name); }
};

// Minimal friend accessor for tests: exposes constEval without widening API surface
class SemanticAnalyzerConstEvalAccessorForTests {
public:
    static bool constEval(const Expr* expr, double& out) { return SemanticAnalyzer::constEval(expr, out); }
};

} // namespace gwbasic
