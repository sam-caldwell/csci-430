# Scope Handling in the GW-BASIC Compiler

This document explains how the compiler models and implements scope, name resolution, and variable lifetime for 
GW-BASIC programs.

Goals:
- Match GW-BASIC’s mostly-global variable semantics so programs behave as expected.
- Keep the implementation simple and explicit, while leaving space for future extensions.
- Make scoping decisions visible in the semantic logs for debugging and tests.

## Language Model (Summary)

- Global by default: Variables in GW-BASIC are global. 
  - Structured blocks (`IF…THEN/ELSE`, `FOR…NEXT`, `WHILE…WEND`) do not introduce local scopes. 
  - `GOSUB/RETURN` does not introduce locals either.
- Implicit declarations: Referencing a variable implicitly declares it if it hasn’t been declared yet.
- Type suffixes: `$` denotes string variables; numeric suffixes (`%`, `!`, `#`) are accepted syntactically but all 
  numeric values are lowered to double in codegen for now. `A` and `A$` are distinct variables.
- Arrays are global: `DIM` defines array shapes globally. `A` (scalar) and `A()` (array) are distinct.
- User functions (`DEF FN`): Parameters are local to the function expression; the rest of names refer to globals.
- COMMON/CHAIN: COMMON marks variables to be preserved across `CHAIN` boundaries.

## Implementation Overview

The compiler uses a two-phase pipeline:

- Semantics (`SemanticAnalyzer`): Resolves names, collects global facts (variables, string literals, line numbers, 
  arrays, user functions), validates simple type/arity/domain rules, and logs scoping events.
- Codegen (`CodeGenerator`): Allocates storage for variables/arrays, lowers statements/expressions to LLVM IR, 
- and applies lifetime policies for `RUN`, `CLEAR`, and `CHAIN`.

Key files:
- `include/basic_compiler/semantics/SemanticAnalyzer.h`
- `src/basic_compiler/semantics/*.cpp`
- `include/basic_compiler/codegen/CodeGenerator.h`
- `src/basic_compiler/codegenerator/*.cpp`

## Name Resolution and Declarations

- Implicit global declarations: The first reference to a variable triggers a declaration in the global scope.
  - Semantics entry points:
    - `SemanticAnalyzer::reference()` declares on first reference.
    - `SemanticAnalyzer::declare()` inserts the symbol into the global scope and the `variables` set.
  - Logging entries: `VarImplicitDecl <name>`, `VarDecl <name>`, `VarRef <name>`.

- Case handling: Identifiers are case-preserving and compared literally today. (Keywords are recognized 
  case-insensitively by the lexer.)

- Type defaults: String-typed variables are detected by `$` suffix or `DEFSTR` letter ranges. 
  DEFINT/DEFSNG/DEFDBL/DEFSTR adjust per-letter defaults; only `DEFSTR` affects storage kind (string vs. numeric) 
  in codegen currently.

- Distinct namespaces by form:
  - `A` (numeric scalar) vs `A$` (string scalar) are separate.
  - `A` (scalar) vs `A()` (array) are separate.

## Scopes and Blocks

- Global scope model: The semantic analyzer maintains a simple lexical scope stack primarily for structure/logging:
  - `enterScope()/exitScope()` are called when analyzing `IF` bodies, `ELSE` bodies, `FOR` bodies, and `WHILE` bodies.
  - However, declarations still land in the global scope. This intentionally matches GW-BASIC: variables “introduced” 
    inside blocks remain visible after the block.
  - Tests validating this behavior: `VarDeclaredInIfBodyVisibleAfter`, `VarDeclaredInForBodyVisibleAfter`, 
    `VarDeclaredInWhileBodyVisibleAfter`.

- Subroutines: `GOSUB/RETURN` does not introduce a new scope. Codegen inlines subroutine bodies at the callsite; 
  variables referenced therein are global.

## DEF FN Functions (Local Parameters)

- Discovery: `DEF FNname(param) = <expr>` is recorded in semantics (`userFunctions` map) keyed by uppercased function 
  name.
- Parameter scoping:
  - Semantics: `currentFnParam_` marks the parameter; references to that identifier inside the body are treated as 
    local (not added to the global variable set).
  - Codegen: Calls are inlined; a temporary binding maps the parameter name to the evaluated argument SSA value via a
    small `bindingStack_`. Variables elsewhere in the function body resolve globally.
- Types: Return type follows function name suffix (`$` means string). Parameter type follows its name suffix.

## Arrays and DIM

- Arrays are declared globally with `DIM`. The semantics pass records `name → length` in `arrays`.
- Codegen allocates an `[N x double]` stack slot for each array on first use (`ensureArrayAllocated`). Indices are 
  numeric and bounds are not currently enforced.

## COMMON and CHAIN (Preservation Across Program Transitions)

- COMMON: `COMMON a, b, c` marks variables to be preserved across a subsequent `CHAIN` in this compiler. Semantics 
  collects the set of COMMON variables and codegen snapshots which COMMONs are “in effect” before each line.
- CHAIN emission: On `CHAIN` the codegen resets non-preserved variables before branching to the new entry point.
  - With the `ALL` flag set, preservation behavior is toggled per GW-BASIC semantics approximation; see 
    `emit_line_block.cpp` for the exact reset conditions used today.
  - File channels and arrays are not closed or reallocated here; the compiler’s CHAIN is a control-flow branch with 
    selective variable resets.

## CLEAR and RUN (Lifetime Reset)

- CLEAR: Emits stores that reset all scalar numeric variables to `0.0` at the point of execution.
  - Current scope of effect: scalars only; arrays, string pointers, file channels, and DATA indices are not cleared. 
    This may evolve.
- RUN: Resets all scalar numeric variables to `0.0` and branches to the program’s first (or specified) line.

## Storage Model in Codegen

- Scalars: Each distinct scalar variable gets an `alloca` (either `double` or `ptr`) on first use
  (`ensureVarAllocated`), initialized to `0.0` (numeric) or `null` (string pointer).
- Arrays: Each array gets an `alloca [N x double]` on first use.
- Strings: String expressions allocate temporary buffers via `malloc` and basic `str*` routines. Variables with `$` 
  hold pointers.

## What’s Not (Yet) Implemented

- Full runtime semantics for `CHAIN` (file handoff, full program overlay) and `CLEAR` (arrays, open files, DATA 
  pointer) are stubbed/simplified.

## How to Inspect Scope Decisions

- Enable semantic logs via `SemanticAnalyzer::setLogPath()`. You’ll see entries like:
  - `Line <n>`
  - `ScopeEnter` / `ScopeExit` (structured blocks)
  - `VarImplicitDecl <name>`, `VarDecl <name>`, `VarRef <name>`
  - `DefFn <name>` and `FnParamRef <param>` references within function bodies

## Pointers for Navigating the Code

- Semantics scope and globals:
  - `include/basic_compiler/semantics/SemanticAnalyzer.h` (scope stack, defaults, results)
  - `src/basic_compiler/semantics/analyze_stmt.cpp` (blocks, COMMON, DEF FN, CLEAR)
  - `src/basic_compiler/semantics/reference.cpp` and `declare.cpp`

- Codegen storage and lifetime rules:
  - `src/basic_compiler/codegenerator/ensure_var_allocated.cpp`
  - `src/basic_compiler/codegenerator/ensure_array_allocated.cpp`
  - `src/basic_compiler/codegenerator/emit_line_block.cpp` (`RUN`, `CLEAR`, `CHAIN`, `GOSUB` inline, etc.)
  - `src/basic_compiler/codegenerator/emit_expr.cpp` (DEF FN parameter binding, built-ins)
