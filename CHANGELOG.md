# Changelog

## 31 Oct 2025

- Initial version.

---

## 01 Nov 2025

### AST & Semantics
- Added LLVM-style RTTI to AST (Node/NodeKind) with `isa`/`dyn_cast`.
- Introduced `SemanticAnalyzer` with scope tracking, type checks, and strict `GOTO`/`GOSUB` validation (now default).
- Enabled string-to-string comparisons (codegen via `strcmp`).
- Refactored codegen/optimizer away from `dynamic_cast`.
- Integrated semantics into the compiler pipeline (`CodeGenerator::setSemantics`).

### Lexer/Parser/AST
- Introduced `CallExpr` AST and parsing of `identifier(...)` with comma-separated args.

### Intrinsics (Math)
- Implemented numeric functions `SQR` (sqrt) and `ABS` with semantic checks (unary, numeric-only).
- Extended intrinsics per GW-BASIC: `SIN`, `COS`, `TAN`, `ATN`, `LOG`, `EXP`, `INT`, `FIX`, `SGN` (arity=1). Domain 
  checks for `SQR`/`SQRT` and `LOG`.
- New intrinsics: `RND` (uniform [0,1) via `drand48`), `CINT` (round), `CSNG` (float trunc/extend), `CDBL` 
  (pass-through).
- Semantics enforce arity=1 and numeric args.

### Codegen
- Declares and emits calls to `@sqrt`, `@fabs`, `@sin`, `@cos`, `@tan`, `@atan`, `@log`, `@exp`, `@round`.
- `CSNG` lowers via `fptrunc`/`fpext`; `CDBL` is passthrough.
- Links with `-lm` on non-Apple platforms.

### Randomness
- Implemented full `RND(x)` semantics and `RANDOMIZE` statement.
  - `RND(x)`: `x=0` returns last value; `x<0` reseeds using `x` and returns next; `x>0` returns next.
  - Implemented via IR helper `@gwb_rnd` and global `@gwb_last_rnd`; helper emitted only when used.
- `RANDOMIZE [expr]` seeds RNG via `srand48`; without expr uses `time(NULL)`.

### Tests
- Added targeted file-stream lexer tests (`std::ifstream`) writing under `build/tmp` to cover `Lexer(std::istream&)`.
- Strengthened semantic analysis tests: IF target validation, numeric-only `FOR` start/end/step, and duplicate line 
  number detection.
- Unit tests cover codegen emission and semantic arity/type errors.
- Exhaustive tests for math intrinsics, including domain-invalid values for `SQR`/`SQRT` and `LOG`.
- E2E tests for `CINT` half-away-from-zero and `RND` range assertions.
- Verified `make clean lint test build demo` completes successfully.

---

## 02 Nov 2025

### Control Flow
- `FOR...NEXT`: Added multi-line support.
  - New AST marker `NextStmt`; parser folding aggregates bodies and validates `NEXT` var name.
- `FOR STEP`: Implemented ascending/descending loops; codegen selects `<=` or `>=` by step sign.
- IF blocks: Added multi-line `IF...THEN` and `IF...THEN...ELSE` with `END IF`.
- `WHILE...WEND`: Added single-line and multi-line forms.

### Lexer/Parser/AST
- Tokens: `ELSE`, `WHILE`, `WEND`.
- AST: `IfBlockStmt`, `ElseStmt`, `EndIfStmt`, `WhileStmt`.
- Parser folding handles THEN/ELSE and WHILE bodies.

### Codegen
- `emitIfBlock` lowers then/else paths; semantics validate bodies.
- `emitWhile` generates cond/body/end blocks.

### Demos
- Added `demos/trig-multi-line-loop.bas` (multi-line FOR loop).
- Added `demos/fibonacci.bas` (multi-line `WHILE...WEND` printing up to 100).

### Tests
- Unit + E2E tests for `FOR`, `IF`, and `WHILE` happy/sad paths.
- Improved scoping tests to validate semantic analysis.

### Constants & Cleanup
- Centralized character/string constants in `include/basic_compiler/Symbols.h` (`CH_SPACE`, `SYM_DEL`, `SYM_LF`, 
  `SYM_TAB`, `SYM_CR`, `CH_SINGLE_QUOTE`, `SYM_NULL`, `CH_DOLLARSIGN`).

---

## 03 Nov 2025

### Demos
- Rewrote `demos/data-arrays-strings-integers-and-floats.bas` to avoid array element expressions and `FOR`-body 
  `READ`s. Uses scalar `READ`s, then stores into arrays; prints scalar sums and string concatenations; compiles 
  cleanly.
- Added demo to `Makefile.d/demo.mk` and restored `chain-level4` demo.

### E2E Workdir & Artifacts
- Adjusted DEF SEG e2e test to `BSAVE`/`BLOAD "build/mem.bin"` so artifacts land under `build/`.
- Left E2E test `WORKING_DIRECTORY` at project root to preserve relative paths for `CHAIN`/`MERGE`/`RUN` tests 
  against `demos/`.

### Codegen/Runtime Declarations
- `emit_header`: now declares `fopen`/`fclose`/`fprintf`/`fread`/`fwrite`, `chdir`, `strncpy`; keeps `printf`/
  `scanf`/`strcpy`/`strcat`/`malloc`/`strlen` helpers.
- Added `.mode_rb`/`.mode_wb` constants for binary I/O.

### Parser/Semantics/Codegen
- Centralized expression string typing predicate; extended `isStringExpr` for built-in string functions and `DEF FN` 
  string returns.
- Collected `DATA`/`READ` literal ids in semantics-driven collection to drive `@gwb_data` emission.

### Pipelines
- Verified `make clean configure lint test demo` completes successfully on macOS (arm64-apple-macOS) with 
  Ninja/Clang 17.

### SCREEN Function & Virtual Screen Buffer
- Implemented `SCREEN(row,col[,z])` as a numeric intrinsic returning the ASCII code (0–255) at the given 1-based 
  coordinates.
  - Semantics: recognized as builtin; accepts 2 or 3 numeric args; third arg reserved (ignored for now).
  - Codegen: reads from a new 80x25 virtual screen buffer `@gwb_screen`; clamps indices to bounds.
- Mirrored `PRINT` output to the virtual screen:
  - Declared `@snprintf` and added IR helper `@gwb_screen_write(ptr,len)` to update `@gwb_screen` and cursor.
  - `PRINT` to stdout now formats into `@gwb_sbuf` and calls `@gwb_screen_write`; file channels (`#n`) unaffected.
- New globals: `@gwb_screen` (2000 bytes), `@gwb_cur_row`/`@gwb_cur_col`, `@gwb_sbuf` (256 bytes).
- Tests: unit (semantics/codegen) and E2E validating ASCII codes after printing `"ABC"`.

---

## 05 Nov 2025

### Language: ON GOTO / ON GOSUB
- Lexer: added `KwOn` token.
- Parser: `ON <expr> GOTO l1[,l2...]` and `ON <expr> GOSUB l1[,l2...]` via `parseOnGotoGosub()`.
- AST: `OnGotoStmt` and `OnGosubStmt` (index expr + targets).
- Semantics: index must be numeric; validates target lines (errors in strict mode; warnings otherwise).
- Codegen: lowers to a `switch i32` on 1-based index; default falls through.
  - `ON GOTO`: cases branch to line labels; default → continuation label.
  - `ON GOSUB`: cases branch to per-case entry labels and inline subroutine; common continuation label.
- Supported in top-level lines, IF/ELSE bodies, and WHILE bodies.
- Tests: unit (parser/semantics), integration (switch targets and default/continuation labels), and E2E (branch/
  subroutine execution; out-of-range fallthrough).

### Tooling/CI
- Pinned LLVM/Clang 17 everywhere and fixed `clang-tidy` in Docker lint.
- Dockerfile: install pinned LLVM/Clang 17 (`clang-17`, `clang-tidy-17`, `llvm-17-tools`, `libc++-17-dev`, 
  `libc++abi-17-dev`, `lld-17`); export `PATH`/`CC`/`CXX` for LLVM 17.
- CMake toolchain (`cmake/toolchain.cmake`):
  - macOS: require Homebrew `llvm@17` explicitly; fail fast if missing.
  - Linux: pin to `/usr/lib/llvm-17`; set `PATH`, `LLVM_PREFIX`, and rpaths.
- Build config (`cmake/BuildConfig.cmake`): apply `-stdlib=libc++` only for C++ compilation to avoid `clang-tidy` 
  errors on C sources; link with libc++ for C++.
- GitHub Actions coverage workflow: macOS installs `llvm@17`; exports `LLVM_PREFIX`/`CC`/`CXX`/`PATH`; Ubuntu uses 
  `apt.llvm.org` `llvm.sh` 17 path; both use unified toolchain.
- `make lint/linux` now succeeds and `make test/linux` passes.

### Style/Tooling
- `.editorconfig` compatibility improvements:
  - Replace `[{Makefile,*.mk}]` with `[Makefile]` and `[*.mk]`.
  - Replace `[{CMakeLists.txt,*.cmake}]` with `[CMakeLists.txt]` and `[*.cmake]`.

### ERASE Statement & Comments
- ERASE arrays: full pipeline implementation and tests.
  - Lexer: added `ERASE` keyword.
  - AST: new `EraseStmt` node with list of array names.
  - Parser: `ERASE name[,name...]` via `parseErase()`; wired into keyword dispatch.
  - Semantics: ERASE removes arrays from the active environment; keeps historical dims so codegen can compute lengths
    for reset without re-enabling usage. Using an array after ERASE now yields: `TypeError: array '<name>' not DIM'd`.
  - Codegen: ERASE emits stores to reset arrays (numeric → 0/0.0; string → null); respects `OPTION BASE` when 
    computing extents; uses existing ensureArrayAllocated helpers.
  - Tests: unit (parser, semantics positive/negative), integration (IR zeroing/null stores), E2E (ERASE + re-DIM 
    behavior).
- Apostrophe `'` inline comment support: strict tests added and behavior validated.

### DATA/READ/RESTORE Overhaul
- Lexer/Parser/AST: `DATA` items now preserve tokenization as quoted strings vs numbers (new `DataItem`).
- Codegen:
  - Removed `atof` dependency in `READ`; now follows GW-BASIC tokenization.
  - Globals: `@gwb_data` (ptrs to string payloads), `@gwb_data_isstr` (flags), `@gwb_data_num` (double values).
  - `READ`: bounds-checks `@gwb_data_idx`, enforces numeric targets cannot consume quoted-string items (runtime error
    dispatched via `ON ERROR` if installed), and loads numeric values from `@gwb_data_num`. String targets store 
    pointers to literal payloads.
  - `RESTORE`: lowers to a store of 0 into `@gwb_data_idx` (no operand variant only); `RESTORE <line>` not supported.
  - Out-of-data: attempting to READ past the end of the DATA table triggers runtime error code 9 and dispatches via 
    `ON ERROR` when installed.
  - IR: removed declaration and all uses of `@atof`.
- Tests: unit (parser updates), integration (presence of `@gwb_data_isstr`/`@gwb_data_num`, absence of `@atof`), and 
  E2E (RESTORE rewind; type-mismatch handling; out-of-data via `ON ERROR`).

### Diagnostics
- Improved semantic error for call-like identifiers with arguments that are not builtins or user functions to treat
  them as array uses, yielding `array '<name>' not DIM'd` when applicable (e.g., after ERASE).

### Arrays
- DIM arrays with types and multidimensional support.
  - Numeric arrays by suffix: `%` (Int16→i32), `&` (Long32→i64), `!` (Single→float), `#` (Double→double).
  - String arrays by suffix `$`: arrays of ptr elements.
  - Multidimensional `DIM A(m,n,...)` lowers to a single alloca of product extents.
  - `OPTION BASE` 0/1 honored for extent calculation and indexing (`ub - base + 1`).
- Usage and type checking in semantics:
  - Arity: number of indices must match number of DIM bounds.
  - Index expressions must be numeric; string indices rejected.
  - Assignments type-checked: numeric arrays accept numeric; string arrays accept string only.
  - Referencing arrays before DIM or after ERASE produces `array '<name>' not DIM'd`.
- Code generation:
  - Ensures per-array stack allocation on first use (numeric/string variants) and reuses allocas.
  - Linearization from multi-index to linear element with GEP; prints use typed formats.
  - Numeric stores emit `fptosi`/`fptrunc`/bit-casts as appropriate for target element type.
  - String array elements are stored as pointers; string operations use `@strncpy`/`@malloc` helpers.
- Runtime behavior helpers:
  - `CLEAR` resets arrays seen before the CLEAR line: numeric to 0/0.0; strings to null.
  - `ERASE` resets only the specified arrays (numeric→0/0.0; strings→null) without re-enabling usage.
  - `READ` into arrays processes `DATA` in order; numeric via precomputed table; string items stored as pointers.
  - `WRITE #n` from string arrays lowers to `fprintf` with format constants.
  - Optionally mirrors runtime bounds errors into `ERR`/`ERL` and respects `ON ERROR` handlers.

### Tooling
- Linter scope narrowed to C++ sources to avoid environment-specific C toolchain header detection issues during
  `clang-tidy` in this sandbox; no functional code changes.

### Notes
- Consumed enough caffeine to make Mötley Crüe seem tame.

### SWAP Statement
- `SWAP x, y` (variables and arrays).
  - Lexer: added `SWAP` keyword.
  - AST: new `SwapStmt` with left/right variable references (scalar or array element) using `ReadTarget`.

---

## 06 Nov 2025

### Compiler Directives (MERGE/CHAIN/RUN)
- MERGE is compile-time only; no runtime codegen. Integration defined and implemented in the CLI/compiler pipeline:
  - Path resolution: `MERGE "file"` resolves relative paths against the including file via `resolvePath(baseFile, rel)`
    and canonicalizes with `weakly_canonical`.
  - Conflicts: merged lines replace duplicates by line number (`appendMergeProgramReplacing` uses replace-or-append
    semantics).
  - Renumbering: MERGE preserves line numbers (no renumber). CHAIN/RUN imports are parsed once per canonical path and
    renumbered by 1000-based regions (`assignBase`, `renumberProgram`). Targets are patched to `base + firstLine` when
    no explicit target is present.
  - Flow: `compileFile*` performs a single pass: detect directives per line, process MERGE by appending parsed lines 
    into the composite, and process CHAIN/RUN by ensuring import, renumbering, and patching target lines. MERGE causes
    no import stack push; CHAIN/RUN do.

### Parser/Control Flow
- NEXT var-list: Implemented `NEXT v1[,v2...]` parsing and folding semantics.
  - Parser accepts comma-separated variable lists; AST `NextStmt` now holds `vars: vector<string>`.
  - Folding enforces order: each listed name must match the current innermost open FOR and closes it (equivalent to 
    `NEXT v1 : NEXT v2 : ...`). Bare `NEXT` still closes one level.

### GOSUB/RETURN
- Clarified behavior: GOSUB is lowered via inlining; nonlocal `RETURN <line>` is not supported.
- Tests: added parser negative test that `RETURN n` is rejected.

### WHILE/WEND
- Added additional tests around nested interactions; EXIT interactions to be considered later as other control 
  statements land.

### Tests/Infra
- New tests:
  - Unit: NEXT var-list (happy and mismatch), RETURN-with-line rejected.
  - E2E: NEXT var-list nested loops print expected pairs.
- Test working directory remains under `build/testrun` for all suites, ensuring artifacts stay within `build/`.

  - Parser: `SWAP varref, varref` parsing wired in `tryParseOtherKeywords`.
  - Semantics: validates both refs exist (arrays DIM'd; indices numeric and correct arity) and types match (both
    numeric or both string). Clear error messages on mismatch.
  - Codegen: emits efficient swap for strings (pointer cross-store) and numerics (loads as double then typed stores 
    back; array element refs include runtime bounds checks).
- Tests: unit (parser + semantics), integration (IR patterns for string vars and numeric array elems), and E2E 
  (numeric and string swaps).

### Numeric Literal Formats
- Lexer: expanded number scanning to support scientific notation with `E/D` exponents (case-insensitive), normalizing 
  `D` to `E` for downstream parsing.
- Added `&` numeric forms beyond hex:
  - `&H[0-9A-F]+` (hex)
  - `&O[0-7]+` (octal)
  - `&[0-7]+` (octal shorthand)
  - `&B[01]+` (binary)
- Emits Integer tokens with decimal lexemes for base-prefixed forms.
- Parser/Codegen: primary numeric parsing and `DATA` number table construction accept scientific notation with `D`.
- Tests: unit (lexer exponent normalization and `&O`/`&`/`&B` decoding), integration (`DATA 1D2` → double 
  `1.000000e+02` in `@gwb_data_num`), and E2E (PRINT behavior) — all green.

### Numeric & Semantics
- Add `VAL`, `LEN` (of string), `INSTR`, and clarify `FIX`/`INT` differences (behavior vs. negatives).
- Domain handling for `SGN`/`SQR`/`SQRT`/`LOG` completed; arity and domains enforced uniformly via `expected_arity`
  (currently returns 1 for all; `SCREEN` is special-cased elsewhere).
- Mixed-type arithmetic conversion rules: expressions are computed in double then stored with truncation/rounding; 
  consider matching GW-BASIC’s rounding/truncation semantics per operator.
- Implement the `INSTR` clamp and the optional `CHR$`/`ASC` strict checks now; add a strict-compat flag for `SQRT` 
  alias behavior.

---

## 07 Nov 2025

### Tests: Coverage Push
- Added optimizer and IR tests to raise coverage without filtering:
  - Optimizer: IF with foldable comparisons flattens to GOTO/removal (`IfCmp_Flattens_ToGotoOrRemoval`).
  - Codegen: AND/OR boolean lowering uses `fcmp` → `and/or i1` → `uitofp`.
  - DEF FN: parameter binding inlines argument SSA; IR contains no `%X` or `%S$` loads; no calls to user functions.
  - RESUME: explicit `RESUME 30` generates handler-flag clear and direct `br label %line30`; E2E validates output order.
- All new tests run under `build/testrun`. No changes to `cmake/Coverage.cmake`.

### INPUT Enhancements
- Parser now supports console `INPUT` variants:
  - `INPUT var[, var ...]`
  - `INPUT ; prompt$, var[, ...]`
  - `INPUT "prompt"; var[, ...]` (prompt literal parsed; runtime prints variable prompts; literal prompts parsed and
    carried in AST).
- Runtime/codegen:
  - Console `INPUT` accepts a variable list; emits one `scanf(%lf)` per numeric variable and stores with proper type 
    conversion.
  - Optional prompt printing via `printf` for prompt variables; literal-prompt printing wired via string table (subject
    to semantics seeding).
  - `LINE INPUT [#n,] var$` implemented using `fgets` into a fixed buffer, newline strip, heap copy, and store to 
    string variable; channel form reads from `@gwb_files`.
- Tests:
  - Unit (parser): var-list parsing; literal/variable prompt forms.
  - Integration (IR): prompt+list emits `printf` and multiple `scanf` calls.
  - E2E: var-list input sums two numbers; prompt-var input feeds and prints value; `LINE INPUT` reads and echoes a line.

### Notes
- Focused on high-signal areas: DEF FN param semantics, RESUME line flow, boolean logic IR paths, and IF-flattening via
  comparison folds. These improve both statement- and expression-level coverage.

### Parser/Lexer: Additional GW-BASIC Keywords (EBNF coverage)
- Added lexer tokens for additional commands from `docs/gw-basic.ebnf` that were previously unrecognized:
  `FILES, NAME, KILL, MKDIR, RMDIR, WIDTH, LOCATE, CLS, PSET, PRESET, PAINT, DRAW, VIEW, WINDOW, BEEP, SOUND, PLAY,
  KEY, PEN, STRIG, TIMER, TRON, TROFF, CONT, LOAD, SAVE, NEW, DELETE, LIST, LLIST, AUTO, RENUM, EDIT, PCOPY, RESET,
  SHELL, ENVIRON, OUT, WAIT`.
- Introduced a lightweight AST node `UnsupportedStmt` to accept these statements in the grammar while implementation
  is pending. The parser consumes tokens up to end-of-statement and creates `UnsupportedStmt` with the keyword name.
- Semantics and codegen treat `UnsupportedStmt` as a no-op and log the occurrence for visibility.
- New unit test `Parser.AdditionalKeywords_ParseAsUnsupported` asserts that representative cases (`FILES`, `CLS`,
  `LOCATE`, `WIDTH`, `BEEP`, `TIMER`) parse successfully as `UnsupportedStmt` and that trailing tokens are swallowed.
  - Expanded to cover more keywords (`SOUND`, `PLAY`, `NAME`, `MKDIR`, `RMDIR`); `CLS` is now implemented (see below) and
    is excluded from the unsupported list in the test.

### CLS
- Implemented `CLS` with a dedicated AST node and end-to-end behavior:
  - Semantics: treated as a no-op for type/flow analysis.
  - Codegen: emits `memset(@gwb_screen, 0, 2000)` and resets `@gwb_cur_row`/`@gwb_cur_col` to zero.
  - Tests:
    - Integration (IR): `CodeGen.CLS_EmitsMemsetAndCursorReset` validates expected IR patterns.
    - E2E: `E2E.CLS_ClearsVirtualScreen` writes a character, calls `CLS`, and prints `SCREEN(1,1)` → `0`.

### CHAIN/RUN and COMMON Clarification
- CHAIN and RUN do not perform file handoff or overlay loading in this compiler. They are lowered to intra-module
  control-flow transfers only:
  - RUN resets all variables/arrays and branches to the first (or target) line.
  - CHAIN resets all non-COMMON variables/arrays to zero/null (unless `ALL` specified) and branches to the target/first
    line. COMMON variables declared before the CHAIN line are preserved across the transfer.
- Preservation across true overlays (external file handoff) will mirror this behavior: only variables declared COMMON
  will be preserved; all others will be reinitialized. Overlay file loading is not yet implemented and remains out of
  scope for this release.

### Metrics
- The new paths integrate with existing metrics (token counting, AST snapshots, IR instruction counts). Unsupported
  statements contribute to token and AST counts but produce no IR.
