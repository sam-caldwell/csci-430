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
DEMO_SRCS := demos/factorial.bas demos/trig.bas demos/sqrt.bas
demo: build
	@set -e; \
	BUILD_ROOT="./build"; \
	COMPILER_BIN="$(BUILD_ROOT)/basic_compiler/basic_compiler"; \
	if [ ! -x "$$COMPILER_BIN" ]; then echo "Compiler not found: $$COMPILER_BIN"; exit 2; fi; \
	UNAMES=$$(uname -s); ARCH=$$(uname -m); TRIPLE=""; \
	if [ "$$UNAMES" = "Darwin" ]; then \
	  if [ "$$ARCH" = "arm64" ]; then TRIPLE=arm64-darwin-macos; \
	  elif [ "$$ARCH" = "x86_64" ]; then TRIPLE=x86_64-darwin-macos; fi; \
	elif [ "$$UNAMES" = "Linux" ]; then \
	  if [ "$$ARCH" = "x86_64" ]; then TRIPLE=x86_64-linux-gnu; \
	  elif [ "$$ARCH" = "aarch64" ] || [ "$$ARCH" = "arm64" ]; then \
	    TRIPLE=aarch64-linux-gnu; \
	  fi; \
	fi; \
	TGT_ARG=""; if [ -n "$$TRIPLE" ]; then TGT_ARG="--target $$TRIPLE"; fi; \
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
	  "$$COMPILER_BIN" "$$SRC" --ll "$$LL" --bc "$$BC" -o "$$BIN" --asm "$$ASM" \
	    $$TGT_ARG --lex-log "$$LEX" --syntax-log "$$SYN" --semantic-log "$$SEM" --log "$$CGN"; \
	done; \
	echo "Demo artifacts written under $$BUILD_ROOT/demos";
