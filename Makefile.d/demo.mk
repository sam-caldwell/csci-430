# File: Makefile.d/demo.mk
#
# Purpose: Build the factorial demo and all related artifacts into build/demos/factorial/.
#
# Targets:
#  - demo: Compiles demo source and emits .ll, .bc, executable, .asm, and logs.
#
# Variables:
#  - DEMO_SRC: path to demo source (default demos/factorial.bas)
#
# Notes:
#  - Uses the built basic_compiler CLI and infers a portable target triple.
#
# Build demo programs into build/demos/<name>/
DEMO_SRCS := demos/factorial.bas \
	demos/trig.bas \
	demos/trig-multi-line-loop.bas \
	demos/sqrt.bas \
	demos/fibonacci.bas \
	demos/run-test.bas \
	demos/chain-test.bas \
	demos/chain-level2.bas \
	demos/chain-level3.bas \
	demos/chain-level4.bas \
	demos/merge-test.bas \
	demos/pass-params.bas \
	demos/use-passed-params.bas \
	demos/run-pass-vars.bas \
	demos/variable-state-test.bas \
	demos/pass-params-all.bas \
	demos/use-passed-params-all.bas \
	demos/data-arrays-strings-integers-and-floats.bas \
	demos/working-with-color.bas \
	demos/graphics-circle.bas \
	demos/conways-life.bas
demo:
	@set -e; \
	BUILD_ROOT="./build"; \
	COMPILER_BIN="$(BUILD_ROOT)/basic_compiler/basic_compiler"; \
	if [ ! -x "$$COMPILER_BIN" ]; then echo "Compiler not found: $$COMPILER_BIN"; exit 2; fi; \
	UNAMES=$$(uname -s); ARCH=$$(uname -m); TRIPLE=""; \
	if [ "$$UNAMES" = "Darwin" ]; then \
	  if [ "$$ARCH" = "arm64" ]; then TRIPLE=arm64-apple-macosx; \
	  elif [ "$$ARCH" = "x86_64" ]; then TRIPLE=x86_64-apple-macosx; fi; \
	elif [ "$$UNAMES" = "Linux" ]; then \
	  if [ "$$ARCH" = "x86_64" ]; then TRIPLE=x86_64-linux-gnu; \
	  elif [ "$$ARCH" = "aarch64" ] || [ "$$ARCH" = "arm64" ]; then \
	    TRIPLE=aarch64-linux-gnu; \
	  fi; \
	fi; \
	TGT_ARG=""; \
	if [ -n "$$TRIPLE" ]; then TGT_ARG="--target $$TRIPLE"; fi; \
	for SRC in $(DEMO_SRCS); do \
	  BN=$$(basename "$$SRC" .bas); \
	  OUT_DIR="$$BUILD_ROOT/demos/$$BN"; \
	  BIN="$$OUT_DIR/$$BN"; \
	  LL="$$OUT_DIR/$$BN.ll"; \
	  BC="$$OUT_DIR/$$BN.bc"; \
	  ASM="$$OUT_DIR/$$BN.asm"; \
	  LEX="$$OUT_DIR/$$BN.lex.log"; \
	  SYN="$$OUT_DIR/$$BN.syntax.log"; \
	  SEM="$$OUT_DIR/$$BN.semantic.log"; \
	  CGN="$$OUT_DIR/$$BN.codegen.log"; \
	  mkdir -p "$$OUT_DIR"; \
	  echo "Compiling $$SRC -> $$OUT_DIR (target=$$TRIPLE)"; \
  "$$COMPILER_BIN" "$$SRC" --metrics --no-logs --ll "$$LL" --bc "$$BC" -o "$$BIN" --asm "$$ASM" \
    $$TGT_ARG; \
	done; \
	echo "Demo artifacts written under $$BUILD_ROOT/demos";

#
# Target: verify-ir
# Purpose: Run a fast lli-based smoke check over all demo bitcode (.bc) to
#          catch invalid IR issues (e.g., dominance errors) early in CI.
#
# Variables:
#  - LLI: path to lli (default derives from LLVM_PREFIX or 'lli' in PATH)
#  - LLI_SMOKE_TIMEOUT: per-demo timeout seconds (default 5)
#  - LLI_SMOKE_MAX_OUTPUT: cap captured stdout/stderr bytes (default 8192)
#
# Behavior:
#  - Builds demos (ensures .bc exists), then runs each with lli.
#  - If a program exits non-zero, the target fails.
#  - If a program runs longer than timeout, it is terminated and treated as OK
#    (goal is IR validity, not program completion).
#
.PHONY: verify-ir
verify-ir: demo
	@set -e; \
	LLI=$(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/lli,lli); \
	if ! command -v "$$LLI" >/dev/null 2>&1; then echo "[verify-ir] 'lli' not found (LLI='$$LLI'). Set LLVM_PREFIX or LLI."; exit 2; fi; \
	TIMEOUT=$${LLI_SMOKE_TIMEOUT:-5}; \
	MAXOUT=$${LLI_SMOKE_MAX_OUTPUT:-8192}; \
	RC=0; \
	for SRC in $(DEMO_SRCS); do \
	  BN=$$(basename "$$SRC" .bas); \
	  BC="./build/demos/$$BN/$$BN.bc"; \
	  if [ ! -f "$$BC" ]; then echo "[verify-ir] Missing bitcode: $$BC"; RC=1; continue; fi; \
	  echo "[verify-ir] lli smoke: $$BC (timeout=$$TIMEOUT s)"; \
	  python3 scripts/lli_smoke.py "$$LLI" "$$BC" "$$TIMEOUT" "$$MAXOUT" || RC=$$?; \
	done; \
	exit $$RC
