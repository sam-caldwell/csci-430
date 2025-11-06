# ToDo List

- REDBULL BEFORE CODING.  Tired Sam is dumb Sam!

## Parser and Grammar Coverage

- LSET/RSET field$ = expr$: fixed-length string field assignment semantics.
 
- Additional PRINT forms: full “PRINT USING” with zone/tabbing and trailing separators (comma/semicolon) behavior.

- INPUT improvements:
  - INPUT var[, var...], INPUT ; prompt$, var[, ...], INPUT "prompt"; var[, ...] (current parser only accepts single 
    var or redirects to file form).
  - LINE INPUT forms already parse, but see runtime gaps below.

- Additional commands listed in docs/gw-basic.ebnf but not recognized in the lexer/token set: FILES, NAME, KILL, MKDIR,
  RMDIR, WIDTH, LOCATE, CLS, PSET, PRESET, LINE (graphics), PAINT, DRAW, VIEW/VIEW PRINT, WINDOW, BEEP/SOUND/PLAY, 
  KEY/KEYn/ON KEY, ON event variants, PEN/STRIG, TIMER ON/OFF, TRON/TROFF, CONT, LOAD/SAVE/NEW/DELETE/LIST/LLIST/AUTO/
  RENUM/EDIT/PCOPY, RESET, SHELL, ENVIRON, OUT, WAIT.

- Add metrics for optimizations in semantics (algebraic simplifications, constant folding, etc.) and codegen 
  optimizations.


## Code Generation: Implementations Missing (parses exist)

- WRITE [#n,] expr[, ...]: parser exists; no codegen lowering yet.
- INPUT #n, var[, ...]: file-input statement parsed; no codegen to `fscanf`/buffer + conversions.
- LINE INPUT [#n,] var$: parser exists; no codegen to read an entire line (channel or stdin) and assign string.
- RUN "file"[, line]: codegen ignores filename and only resets variables and branches; no handoff/loading semantics.
- MERGE "file": compile-time directive only; codegen is a no-op; finalize expected behavior or tooling integration.
- DEF SEG [= expr]: treated as semantic/logging only; codegen is a no-op yet memory ops (POKE/PEEK/CALL/BLOAD/BSAVE) 
  reference @gwb_seg. Need runtime to set @gwb_seg (store casted value) when provided.
- DEF USRn = expr and USR(): DEF USR is a no-op; USR(arg) returns arg identity in expressions. Define callout ABI or 
  trap and optional index dispatch.

## File I/O Semantics

- OPEN "name" FOR ... AS #n:
  - Only INPUT and OUTPUT modes are implemented. Missing: APPEND, RANDOM, BINARY; missing LEN=, ACCESS/LOCK, device forms
    (COM/LPT/CONS), and error handling.
  - Channel range checking and duplicate-open policy unspecified; currently simple table [16 x ptr].
- CLOSE #n: basic support exists; missing CLOSE without argument (close all) semantics and error cases.
- PRINT #n, ...: partial support; double-check format/USING enforcement and trailing separator behavior vs. GW-BASIC 
  zones.
- WRITE #n, ...: see the codegen gap above.
- INPUT #n / LINE INPUT #n: see codegen gaps above; require numeric parsing with separators, string quoting rules, 
  EOF behavior.

## Control Flow and Blocks

- IF block support exists, but missing single-line THEN/ELSE statement lists and THEN/ELSE GOTO forms.
- GOSUB/RETURN implemented via inlining; `RETURN <line>` nonlocal form not supported.
- FOR/NEXT: NEXT var-list form missing; verify semantics for mixed variable names and nested loops per spec.
- WHILE/WEND implemented; consider EXIT loops and interactions once other control statements land.

## Built-in Functions Coverage and Types

- Numeric: add VAL, LEN (of string), INSTR, FIX/INT differences (behavior vs. negatives), SGN/SQR/SQRT/log domain
  handling done; ensure arity and domains enforced uniformly via `expected_arity` (currently returns 1 for all; 
  SCREEN is special-cased elsewhere).
- String: add STR$, STRING$, SPACE$, LTRIM$/RTRIM$/MID$ statement vs. function nuances, LEFT$/RIGHT$ complete; 
  verify types/arity in semantics.
- Update `isKnownNumericFunction`/`isKnownStringFunction` to reflect the above; expand tests.

## Type System and Conversions

- Variable kind defaults and suffixes are honored, but:
  - Default kinds by DEFxxx only affect first-letter currently; document and test crossing interactions with
    explicit suffixes.
  - Mixed-type arithmetic conversion rules (Single vs. Double, Int16/Long promotion) are simplified: expressions are 
    computed in double then stored with truncation/rounding; consider matching GW-BASIC’s rounding/truncation semantics
    per operator.
  - PRINT USING requires a string format; semantics do not enforce that `format` is string.

## Scope and COMMON Behavior

- COMMON variables tracked and preserved across CHAIN, but CHAIN/RUN file handoff is not implemented; clarify 
  preservation across true overlays once implemented.
- CLEAR is line-scoped to “vars/arrays seen before” to avoid crossing CHAIN boundaries; verify against GW-BASIC’s 
  memory model (string space, array descriptors, file buffers).

## Diagnostics and Robustness

- `expected_arity()` returns 1 for all intrinsics; replace with a concrete table and unify with analyzer checks 
  (SCREEN special-cased today).
- Improve error messages and warnings for file I/O (open failures, invalid channels), graphics stubs, and unsafe 
  memory ops (PEEK/POKE/BLOAD/BSAVE/CALL).

## Graphics

- SCREEN is not initializing a graphics window.
- SCREEN initializes a readiness flag; CIRCLE lowers to a no-op stub when ready. Missing: CLS, 
  PSET/PRESET/LINE/PAINT/DRAW, VIEW/WINDOW, PALETTE, and proper page switching.

## Tooling/Directives

- MERGE is treated as a compile-time directive only; define integration with CLI/compiler phases (include path 
  resolution, conflicts, and renumbering strategies).

## Tests

- Add unit tests for the above once implemented: WRITE codegen, INPUT#/LINE INPUT codegen semantics, DEF SEG 
  runtime effect on @gwb_seg, RUN/CHAIN file overlay behavior, OPTION BASE effects on DIM/array indexing, and 
  exponent/logical operator parsing and lowering.
