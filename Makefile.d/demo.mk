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
# Build the factorial demo into build/demos/factorial/
DEMO_SRC := demos/factorial.bas
demo: build
	@set -e; \
	BUILD_ROOT="./build"; \
	OUT_DIR="$(BUILD_ROOT)/demos/factorial"; \
	BIN_NAME="factorial"; \
	BIN="$$OUT_DIR/$$BIN_NAME"; \
	LL="$$OUT_DIR/$$BIN_NAME.ll"; \
	BC="$$OUT_DIR/$$BIN_NAME.bc"; \
	ASM="$$OUT_DIR/$$BIN_NAME.asm"; \
	LEX="$$OUT_DIR/$$BIN_NAME.lex.log"; \
	SYN="$$OUT_DIR/$$BIN_NAME.syntax.log"; \
	SEM="$$OUT_DIR/$$BIN_NAME.semantic.log"; \
	CGN="$$OUT_DIR/$$BIN_NAME.codegen.log"; \
	COMPILER_BIN="$(BUILD_ROOT)/basic_compiler/basic_compiler"; \
	if [ ! -x "$$COMPILER_BIN" ]; then echo "Compiler not found: $$COMPILER_BIN"; exit 2; fi; \
	mkdir -p "$$OUT_DIR"; \
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
	echo "Compiling $$DEMO_SRC -> $$OUT_DIR (target=$$TRIPLE)"; \
	"$$COMPILER_BIN" "$(DEMO_SRC)" --ll "$$LL" --bc "$$BC" -o "$$BIN" --asm "$$ASM" \
	  $$TGT_ARG --lex-log "$$LEX" --syntax-log "$$SYN" --semantic-log "$$SEM" --log "$$CGN"; \
	echo "Demo artifacts written to $$OUT_DIR";
