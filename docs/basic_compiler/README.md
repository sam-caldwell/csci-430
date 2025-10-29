# GW-BASIC to LLVM IR Compiler

>
> (Disclaimer: this is a planning document. Delete this later on!)
>

This project implements a simple, educational GW-BASIC compiler in modern C++ (C++23). It reads a GW-BASIC source file
and emits LLVM IR text (`.ll`). The compiler is intentionally small and approachable, demonstrating the full pipeline:
lexing, parsing, and code generation.

## Scope: 
This is not a complete, production-ready GW-BASIC implementation. It supports a practical subset that is
sufficient to demonstrate compilation to LLVM IR and to write small programs:

 - Line-numbered programs with execution flowing to the next line unless redirected
 - Statements: `LET`, assignment without `LET`, `PRINT` (string and numeric), `IF <comparison> THEN <line>`, 
   `GOTO <line>`, `END`, `REM` comments and `'` comments
 - Expressions: numeric literals, variables, unary `+`/`-`, `+ - * /`, parentheses
 - Comparisons: `=  <>  <  <=  >  >=`

Generated IR uses opaque pointers (`ptr`) and declares `@printf` for I/O. Numeric output uses `%f\n` and string 
output uses `%s\n`.

## Directory layout:

- `include/basic_compiler/`: headers for Lexer, Parser, AST, CodeGenerator, and public `Compiler` API
- `src/basic_compiler/`: C++ sources and CLI (`main.cpp`)
- `docs/basic_compiler/README.md`: this document
- `test/{unit,integration,e2e}`: GoogleTest suites

## Build and run:

1. Ensure Homebrew LLVM 17 is available (CMake prefers `llvm@17`).
2. Configure and build as usual:
   - `cmake -S . -B build/cmake-build-debug`
   - `cmake --build build/cmake-build-debug --parallel`
3. Run the compiler:
   - Show help: `-h` or `--help`
   - Emit LLVM IR (.ll): `build/basic_compiler/basic_compiler examples/hello.bas -ll hello.ll` (alias: `--ll`)
   - Emit LLVM bitcode (.bc): `--bc hello.bc`
   - Emit native executable: `-o hello.out`
   - Emit ARM64 assembly (text): `--asm hello.arm64.asm`
     - The `.asm` file begins with a comment header indicating the source filename and target OS/CPU (derived from 
       `--target`). The header uses a comment token appropriate to supported targets:
       - GNU/ELF, Mach-O (x86_64, etc.): `#`
       - AArch64/ARM64 (GAS): `//`
       Example (Linux x86_64): `# Source: factorial.bas | Target: os=linux, cpu=x86_64 (triple=x86_64-linux-gnu)`
       Example (ARM64 macOS): `// Source: factorial.bas | Target: os=darwin, cpu=arm64 (triple=arm64-darwin-macos)`
   - Or pipe IR to stdout without `-ll/--bc/-o/--asm`.

## Supported targets:
- Architectures: `x86_64`, `arm64`/`aarch64`
- OS: Linux and macOS (Darwin).

## Stretch Goals (Targets):
- WebAssembly? (It would be nice...but we need to use restraint).
- MIPS?

## Example BASIC program:
```
10 LET A = 1+2*3
20 PRINT A
30 IF A > 3 THEN 50
40 PRINT "Done"
50 END
```

## Testing:
- Unit tests cover the lexer and parser
- Integration tests validate IR structure for small programs
- End-to-end tests compile a BASIC program to IR, invoke `clang` to produce a native binary, run it, and check its 
  output

## Limitations and future work:
- No string concatenation or numeric input
- Error handling is minimal and focused on clarity
