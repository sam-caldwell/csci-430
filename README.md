# GW-BASIC Compiler (basic_compiler)

This project develops a GW-BASIC compiler which emits LLVM IR, byte code, native assembly, and native executables.
This is designed for reproducible builds and detailed phase logging for compiler development.

Code repo: https://github.com/sam-caldwell/csci-430

## Supported OS/Architectures

### CPU Architectures

- x86_64
- arm64/aarch64

### Operating System

- Linux
- macOS (Darwin)

## Prerequisites

- Tools: `llvm@17` (clang/clang++), `cmake`, `ninja`, `make`, `googletest`.
- Install (macos): `brew install llvm@17 cmake ninja zip`.

## Automation

| make target  | description                                |
|--------------|--------------------------------------------|
| `make help`  | List all make targets                      |
| `make clean` | Clean build artifacts.                     |
| `make lint`  | Run linter.                                |
| `make test`  | Run tests (unit, integration, end-to-end)  |
| `make build` | Build the compiler.                        |
| `make demo`  | Build a demo program (demos/factorial.bas) |

> All Build artifacts are placed in `build/`
> To Run the demo program, `cd build/demos/factorial` and run `./factorial` then enter a number (e.g., 5)
> ```text
> % ./factorial 
> 5
> 120.000000
>```
> As we see, the result of the factorial function is printed (120.000000).

## Compiler Usage

- Binary: `build/basic_compiler/basic_compiler`
- Synopsis:
    - `basic_compiler <input.bas> [-ll|--ll <file>] [--bc <file>] [-o <exe>] [--asm <file>] [--target <triple>] 
          [--lex-log <file>] [--syntax-log <file>] [--semantic-log <file>] [--log <file>]`
    - Help: `basic_compiler -h` or `basic_compiler --help`
- Notes:
    - Assembly files begin with a header comment line:
      `Source: <file> | Target: os=<os>, cpu=<arch> (triple=<triple>)`
      using an architecture-appropriate comment leader.
    - If log paths are omitted, logs default next to the input with matching extensions.
    - Bitcode/EXE/ASM require `clang` to be available; the build injects its path as `CLANG_PATH`.

## Supported Targets

- Whitelist only: x86_64 and arm64/aarch64 on Linux, macOS (Darwin)
- Windows/MSVC, ARM32, WebAssembly are not supported.
- Set explicitly via `--target <triple>`, e.g. `x86_64-unknown-linux-gnu`, `aarch64-apple-macos`.

## Artifacts And Logs

- IR: `.ll` human-readable LLVM IR.
- Bitcode: `.bc` machine IR, useful for linking or analysis.
- Assembly: `.asm` with a header comment reflecting source and target; dialect matches target triple.
- Executable: platform-native binary produced by `clang`.
- Logs: phase logs capture tokens, syntax steps, semantic validations, and codegen mappings.
  - Lexical (`.lex.log`): token stream with locations.
  - Syntax (`.syntax.log`): recursive-descent parse events by line and node.
  - Semantic (`.semantic.log`): symbol declarations/references, loop structure, and scope enter/exit.
  - Codegen (`.codegen.log`): mapping from AST nodes to emitted LLVM IR.

## Architecture Notes

- Parser: hand-written recursive descent (`Parser`) implementing a classic expression precedence ladder
  (`parseExpression` → `parseComparison` → `parseTerm` → `parseFactor` → `parseUnary` → `parsePrimary`).
- AST: lightweight hierarchy with LLVM-style RTTI
  - Each node carries a `NodeKind`; `isa<>`/`dyn_cast<>` helpers are available in `basic_compiler/ast/RTTI.h`.
  - `Expr`/`Stmt` are abstract bases; concrete nodes implement `classof()` for fast kind checks.
- Semantics: dedicated pass (`SemanticAnalyzer`) performs symbol and scope analysis prior to codegen
  - Records variable declarations/uses with simple, extensible scope tracking.
  - Emits a semantic log independent from code generation.

## Tips

- Use `--target` to control the intended output architecture/OS; defaults to host-appropriate when omitted in demo.
- For best portability, emit IR/bitcode and link on the destination host with an appropriate `-target`.

## Troubleshooting

| Problem                                    | Solution                                                     |
|--------------------------------------------|--------------------------------------------------------------|
| “CLANG_PATH not defined”                   | Use `make build` so the project has `CLANG_PATH` injected.   |
| “unsupported target triple”                | Only x86_64 and arm64/aarch64 for Linux/macOS are accepted.  |
| Mismatched target warnings when linking IR | Re-run with `--target <triple>` appropriate for your system. |
