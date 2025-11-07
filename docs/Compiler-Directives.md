# Compiler Directives: MERGE, CHAIN, RUN

This compiler treats import-like statements in GW-BASIC in a strictly compile-time manner where appropriate and 
defines a clear integration with the CLI/compiler phases.

## Scope
- `MERGE "file"`: compile-time only. Appends lines from the referenced program into the current program; no runtime 
  code is generated.
- `CHAIN "file"[, line][, ALL]` and `RUN "file"[, line]`: compile-time import for path resolution and target patching; 
  runtime lowers to a branch into the target region after resetting variables per semantics.

## Path Resolution
- Relative paths are resolved against the including file’s directory. Both relative and absolute paths are 
  canonicalized using `std::filesystem::weakly_canonical`.
- The root file path is canonicalized once when compiling from a file and propagated through directive detection.

## MERGE Behavior
- When a `MERGE "file"` is encountered, the referenced program is parsed and its lines are appended into the composite
  program using replace-or-append semantics:
  - If a merged line number matches an existing number, it replaces the existing line (conflict resolution by replace).
  - Otherwise, the line is appended.
- `MERGE` does not push a new frame onto the import stack; the merged lines are not scanned recursively for further 
  import processing during this pass.
- `MERGE` never renumbers imported lines; line numbers are preserved as written.

## CHAIN/RUN Behavior
- Each referenced program is parsed at compile time and assigned a region base in 1000-line increments (1000, 2000, …)
  per canonical path. Multiple references to the same canonical path share the same base.
- The imported program is renumbered by adding its base to each line. The first original line number is recorded.
- Targets in `CHAIN`/`RUN` are patched at compile time:
  - If the statement includes an explicit target, the runtime branch goes to `base + target`.
  - If no explicit target is provided, the runtime branch goes to `base + firstLine` of the imported program.
- Runtime behavior resets variables/arrays according to `CHAIN`/`RUN` semantics; `CHAIN` preserves `COMMON` variables
  unless `ALL` is specified.

## Compiler/CLI Pipeline Integration
- Both `compileFile()` and `compileFileWithPhaseLogs()` perform a single pass over the root program lines, handling 
  directives per line:
  1) Detect `MERGE`/`CHAIN`/`RUN` on a line and resolve the include path against the current file.
  2) `MERGE`: parse the referenced file and append its lines (replacing duplicates) into the composite program.
  3) `CHAIN`/`RUN`: ensure the imported program is present in the import table (parse + renumber on first encounter) 
     and patch the directive’s target line accordingly.
  4) Non-directive lines (or the directive line after patching) are replaced/appended into the composite output.
- After the pass, the composite program is analyzed and lowered to LLVM IR identically for string and file inputs.

## Key Helper Functions
- `resolvePath(baseFile, rel)`: resolves relative include paths against the current file.
- `canonicalPath(p)`: canonicalizes paths for stable map keys.
- `replaceOrAppendLine(dst, line, replace)`: conflict resolution by line number.
- `appendMergeProgramReplacing(dst, src)`: merging helper (always replace on duplicates).
- `assignBase(canon, cur, imported)`: computes a 1000-based region for an imported program.
- `renumberProgram(prog, base, outMinLine)`: renumbers lines and reports the smallest original line.
- `patchTargetsForChainOrRun(line, {base,first}, isChain)`: patches `CHAIN`/`RUN` target lines.

## Notes
- `MERGE`’s compile-time-only behavior and `CHAIN`/`RUN`’s base+patching scheme keep runtime code simple and keep 
  artifacts deterministic.
- `MERGE` of programs that themselves contain `CHAIN`/`RUN` statements preserves those statements as written; any 
  handoff semantics still depend on the runtime lowering of `CHAIN`/`RUN`.
