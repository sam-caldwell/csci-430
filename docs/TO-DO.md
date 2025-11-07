# ToDo List

- REDBULL BEFORE CODING.  Tired Sam is dumb Sam!

## Parser and Grammar Coverage

- Additional commands listed in docs/gw-basic.ebnf but not recognized in the lexer/token set: FILES, NAME, KILL, MKDIR,
  RMDIR, WIDTH, LOCATE, CLS, PSET, PRESET, LINE (graphics), PAINT, DRAW, VIEW/VIEW PRINT, WINDOW, BEEP/SOUND/PLAY, 
  KEY/KEYn/ON KEY, ON event variants, PEN/STRIG, TIMER ON/OFF, TRON/TROFF, CONT, LOAD/SAVE/NEW/DELETE/LIST/LLIST/AUTO/
  RENUM/EDIT/PCOPY, RESET, SHELL, ENVIRON, OUT, WAIT.

## File I/O Semantics

- OPEN "name" FOR ... AS #n:
  - Only INPUT and OUTPUT modes are implemented. Missing: APPEND, RANDOM, BINARY; missing LEN=, ACCESS/LOCK, device 
    forms (COM/LPT/CONS), and error handling.
  - Channel range checking and duplicate-open policy unspecified; currently simple table [16 x ptr].
- CLOSE #n: basic support exists; missing CLOSE without argument (close all) semantics and error cases.
- PRINT #n, ...: partial support; double-check format/USING enforcement and trailing separator behavior vs. GW-BASIC 
  zones.
- WRITE #n, ...: see the codegen gap above.
- INPUT #n / LINE INPUT #n: see codegen gaps above; require numeric parsing with separators, string quoting rules, 
  EOF behavior.

## Built-in Functions Coverage and Types

- String: add STR$, STRING$, SPACE$, LTRIM$/RTRIM$/MID$ statement vs. function nuances, LEFT$/RIGHT$ complete; 
  verify types/arity in semantics.
- Update `isKnownNumericFunction`/`isKnownStringFunction` to reflect the above; expand tests.

## Type System and Conversions

- Variable kind defaults and suffixes are honored, but:
  - Default kinds by DEFxxx only affect first-letter currently; document and test crossing interactions with
    explicit suffixes.

## Scope and COMMON Behavior

- COMMON variables tracked and preserved across CHAIN, but CHAIN/RUN file handoff is not implemented; clarify 
  preservation across true overlays once implemented.
- CLEAR is line-scoped to “vars/arrays seen before” to avoid crossing CHAIN boundaries; verify against GW-BASIC’s 
  memory model (string space, array descriptors, file buffers).

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

## Diagnostics and Robustness

- Improve error messages and warnings for file I/O (open failures, invalid channels), graphics stubs, and unsafe 
  memory ops (PEEK/POKE/BLOAD/BSAVE/CALL).

## Graphics

- SCREEN is not initializing a graphics window.
- SCREEN initializes a readiness flag; CIRCLE lowers to a no-op stub when ready. Missing: CLS, 
  PSET/PRESET/LINE/PAINT/DRAW, VIEW/WINDOW, PALETTE, and proper page switching.

## Tests

- Add unit tests for the above once implemented: WRITE codegen, INPUT#/LINE INPUT codegen semantics, DEF SEG 
  runtime effect on @gwb_seg, RUN/CHAIN file overlay behavior, OPTION BASE effects on DIM/array indexing, and 
  exponent/logical operator parsing and lowering.

## Bug Reports

