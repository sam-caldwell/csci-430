# ToDo List

- REDBULL BEFORE CODING.  Tired Sam is dumb Sam!

## Type System and Conversions
- COMMON variables tracked and preserved across CHAIN, make sure CHAIN/RUN file handoff is not implemented; clarify
  preservation across true overlays once implemented.
- Variable kind defaults and suffixes are honored, but:
  - Default kinds by DEFxxx only affect first-letter currently; document and test crossing interactions with
    explicit suffixes.
- Improve semantic optimizations (e.g., add more algebraic simplifications where possible)

## Code Generation: Status (previously missing)
- WRITE [#n,] expr[, ...]: Implemented. stdout uses `printf` and printer/file channels use `fprintf`/`snprintf`.
- INPUT #n, var[, ...]: Implemented. Lowers to `fscanf(FILE*, "%lf", &tmp)` per variable with numeric storage.
- LINE INPUT [#n,] var$: Implemented. Console path uses `scanf("%255[^\n]%*c", buf)`; channel path uses `fgets` and newline trim; copies to heap and assigns.
- RUN "file"[, line]: Compile-time path resolves and patches target; runtime resets variables and branches within the composite program (no loader overlay).
- MERGE "file": Compile-time directive implemented; appends imported lines replacing duplicates; no runtime codegen.
- DEF SEG [= expr]: Implemented. Stores casted integer into `@gwb_seg` (or 0 when omitted) used by POKE/PEEK/CALL/BLOAD/BSAVE.
- DEF USRn = expr and USR(): DEF USR remains a no-op; `USR(x)` returns the numeric argument (identity) in expressions.

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


## Implement the Open Language Features (from gw-basic.ebnf)
The table below tracks features defined in docs/gw-basic.ebnf that are not yet implemented or only partially implemented in the current repository. Percent complete is a rough pipeline estimate (Lexer, Parser, Semantics, Codegen ~25% each):

| Feature                     | Description of the feature / future work                                                                                                                                  | % Complete |
|-----------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-----------:|
| OPEN "COMn:"                | Serial port open (COM) with device options; validate and map to host serial APIs.                                                                                         |         0% |
| COM(n) ON/OFF/STOP          | Enable/disable/stop COM event trapping; wire into ON COM dispatcher.                                                                                                      |         0% |
| FIELD #n, len AS var$       | Random-access record layout binding to string buffers.                                                                                                                    |         0% |
| GET #n, [rec]               | Random-access file GET (structured reads per FIELD layout).                                                                                                               |         0% |
| PUT #n, [rec]               | Random-access file PUT (structured writes per FIELD layout).                                                                                                              |         0% |
| LOCK/UNLOCK #n              | Advisory locks for file records or ranges.                                                                                                                                |         0% |
| IOCTL / IOCTL$              | Device control; pass-through of control codes/strings to devices/channels.                                                                                                |         0% |
| PSET/PRESET/LINE (graphics) | Pixel draw, erase, and line drawing. Honor current screen mode, colors, and clipping.                                                                                     |        25% |
| PAINT                       | Flood fill region from a point with color; optional border color.                                                                                                         |        25% |
| DRAW                        | Turtle-like drawing language parsing/execution onto current page.                                                                                                         |        25% |
| VIEW / VIEW PRINT / WINDOW  | Graphics viewport/window setup; map text viewport (VIEW PRINT) and coordinate transforms (WINDOW).                                                                        |        25% |
| PALETTE                     | Palette index/value programming and palette USING.                                                                                                                        |         0% |
| SOUND                       | Simple tone generation with frequency/duration.                                                                                                                           |        25% |
| PLAY                        | Music macro language parsing/tempo/channel; schedule tones.                                                                                                               |        25% |
| KEY / KEY(n) / ON KEY       | Keyboard management, binding handlers (ON KEY) and KEY toggles.                                                                                                           |        25% |
| PEN                         | Light pen status and event trap setup.                                                                                                                                    |        25% |
| STRIG                       | Joystick trigger and event trapping.                                                                                                                                      |        25% |
| TIMER ON/OFF/STOP           | Timer event trap enable/disable and polling.                                                                                                                              |        25% |
| TRON/TROFF                  | Trace execution on/off; integrate with logger and diagnostics hooks.                                                                                                      |        25% |
| CONT                        | Continue execution after a break/STOP (debug flow).                                                                                                                       |        25% |
| LOAD/SAVE                   | Load/Save BASIC program files; integrate with compiler front-end IO.                                                                                                      |        25% |
| NEW                         | Clear program from memory.                                                                                                                                                |        25% |
| DELETE                      | Delete program line ranges.                                                                                                                                               |        25% |
| RENUM                       | Renumber program lines with dependency updates.                                                                                                                           |        25% |
| EDIT                        | Line editor invocation.                                                                                                                                                   |        25% |
| PCOPY                       | Page copy (graphics pages).                                                                                                                                               |        25% |
| RESET                       | Reset communications/files/devices.                                                                                                                                       |        25% |
| OUT                         | Port OUT (write to hardware I/O address).                                                                                                                                 |        25% |
| WAIT                        | Port WAIT (poll for bit pattern at I/O address).                                                                                                                          |        25% |
| LSET/RSET                   | Left/right-justified assignment into fixed-length strings/fields.                                                                                                         |         0% |
| DEF USR / USR()             | Indirect call-out. USR(x) returns x (identity). DEF USR remains a no-op; ABI/callback remains TODO.                                                                       |        75% |
| RUN "file"[,line]           | Program overlay/transfer. Current codegen resets variables and branches; add file handoff/loading.                                                                        |        60% |
| CHAIN [MERGE]               | Partial: resets state and branches + DATA index regioning. Add file load, COMMON preservation rules.                                                                      |        60% |
| INP(addr)                   | Hardware port input function. Add intrinsic lowering and safety stub on non-PC targets.                                                                                   |         0% |
| POINT/PMAP/POS              | Graphics/text coordinate queries and mapping.                                                                                                                             |         0% |
| IOCTL$                      | Device status/data query as string.                                                                                                                                       |         0% |

Notes
- INKEY$ is currently stubbed as empty string; treat as partial until keyboard polling is supported.
