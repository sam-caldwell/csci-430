# Contributing

This guide covers the basics to get you set up, make changes, and submit them smoothly.

## Quick Start

1. Fork the repo and create a topic branch:
   - Example: `feature/multiline-if`, `fix/while-wend-edgecase`
2. Install prerequisites (see README for details) and build:
   - `make configure`
   - `make build`
3. Run tests and linters locally before opening a PR:
   - `make lint`
   - `make test`
4. Try the demos to sanity‑check end‑to‑end output:
   - `make demo`

Build artifacts are written under `build/`. The compiler binary is `build/basic_compiler/basic_compiler`.

## Development Workflow

- Keep changes focused and incremental; avoid unrelated refactors in the same PR.
- Include tests with behavior changes (parser/semantics/codegen and e2e as appropriate).
- Update documentation when user‑visible features or flags change.
- Update `CHANGELOG.txt` under the appropriate date when behavior changes are user‑facing.
- Ensure `make clean configure lint test build demo` completes successfully.

## Coding Guidelines

- Follow the existing code style and patterns used in the repository. Keep files small, simple, and readable.
- Prefer clear, small functions; the linter checks include a one‑function‑per‑file static rule.
- Keep warnings at zero; do not introduce new build or demo warnings.
- Tests should be deterministic and fast; skip E2E gracefully when `clang` is unavailable 
  (see existing tests for patterns).

## Tests

- Unit tests live under `test/basic_compiler/unit/`.
- Integration and end‑to‑end tests live under `test/basic_compiler/integration/` and `test/basic_compiler/e2e/`.
- Run all tiers locally with `make test`. Use `make unit` for just unit tests.
- If you add language features, cover:
  - parser shape (including error cases),
  - semantic/scoping behavior,
  - codegen/IR shape,
  - and an e2e run where appropriate.
- Build/Test artifacts (e.g., logs) are written under `build/<project>/`.

## Demos

- Demo sources live in `demos/`. Build all demos with `make demo`.
- Demo artifacts (binary, `.ll`, `.bc`, `.asm`, and logs) are written under `build/demos/<name>/`.

## Submitting Changes

- Use clear commit messages describing the problem and the approach.
- Open a Pull Request with:
  - a concise description of the change and motivation,
  - links to related issues (if any),
  - notes on testing performed and any caveats.
- Small, self‑contained PRs are reviewed and merged faster.

## Reporting Issues

- Include reproduction steps, expected vs. actual behavior, and environment details.
- `make version` prints toolchain/OS details that help triage.
- If applicable, include a minimal BASIC program that triggers the issue.

## License

By contributing, you agree that your contributions will be licensed under the same license as the project.

Thanks again for helping make the project better!

