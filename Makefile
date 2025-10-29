# File: Makefile
# (c) 2025 Sam Caldwell.  All Rights Reserved.
# Purpose: Convenience wrapper for configure/build and environment reporting.
#          Provides 'build', 'clean', and 'version' targets for developers.
.PHONY: clean build version tree zip lint test help demo
.DEFAULT_GOAL := help

# Removes the artifact directory and recreates it
clean:
	@echo "Cleaning build/ directory..."
	-@rm -rf build build-basic cmake-build-* || true
	@mkdir -p build
	@echo "Recreated build/"

# Configuration
CMAKE ?= cmake
# Prefer Ninja if available, otherwise fall back to Unix Makefiles
GENERATOR ?= $(shell if command -v ninja >/dev/null 2>&1; then echo Ninja; else echo "Unix Makefiles"; fi)
TOOLCHAIN ?= cmake/Toolchain-HomebrewLLVM.cmake
# Only pass a toolchain if the file exists
TOOLCHAIN_FLAG := $(if $(wildcard $(TOOLCHAIN)),-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN),)
CONFIG ?= Debug
# Derive BUILD_DIR from CONFIG unless overridden by the user
BUILD_ROOT := build
BUILD_SUBDIR := $(shell echo $(CONFIG) | tr A-Z a-z)
BUILD_DIR ?= $(BUILD_ROOT)/cmake-build-$(BUILD_SUBDIR)

# Parallel build settings: detect CPU cores cross-platform
# Allows override: make build NUM_CPUS=8
NUM_CPUS ?= $(shell \
  N=""; \
  if command -v sysctl >/dev/null 2>&1; then N=$$(sysctl -n hw.ncpu 2>/dev/null || true); fi; \
  if [ -z "$$N" ] && command -v nproc >/dev/null 2>&1; then N=$$(nproc 2>/dev/null || true); fi; \
  if [ -z "$$N" ] && command -v getconf >/dev/null 2>&1; then N=$$(getconf _NPROCESSORS_ONLN 2>/dev/null || true); fi; \
  if [ -z "$$N" ]; then N=1; fi; \
  echo $$N)

# Build all targets via CMake/Ninja (auto-configure if needed)
build:
	@if [ ! -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG) -DCMAKE_BUILD_TYPE=$(CONFIG); \
	else \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG); \
	fi
	@echo "Building with $(NUM_CPUS) parallel jobs..."
	@$(CMAKE) --build $(BUILD_DIR) -v -- -j$(NUM_CPUS)

# Tree view of repository (requires 'tree')
TREE_ROOT ?= .
TREE_IGNORES ?= .git|cmake-build-*|build|build-*|out|.idea|CMakeFiles|Testing|__pycache__
tree:
	@command -v tree >/dev/null 2>&1 || { echo "tree not found. Install with: brew install tree"; exit 1; }
	@echo "Repository tree (excluding: $(TREE_IGNORES))"
	@tree -a -I "$(TREE_IGNORES)" "$(TREE_ROOT)"

# Zip the repository (excluding build and VCS/IDE artifacts)
ZIP_BASE := $(notdir $(CURDIR))
ZIP_NAME ?= $(ZIP_BASE).zip
ZIP_EXCLUDES ?= \
  ".git/*" \
  ".zip/*" \
  "build/*" \
  "build-*/*" \
  "cmake-build-*/*" \
  "out/*" \
  ".idea/*" \
  "CMakeFiles/*" \
  "Testing/*" \
  "$(ZIP_NAME)" \
  "*.o" "*.obj" "*.a" "*.so" "*.dylib" "*.dll"

zip:
	@command -v zip >/dev/null 2>&1 || { echo "zip not found. Install with: brew install zip"; exit 1; }
	@echo "Creating $(ZIP_NAME) ..."
	@rm -f "$(ZIP_NAME)"
	@zip -r -q "$(ZIP_NAME)" . -x $(ZIP_EXCLUDES)
	@echo "Wrote $(ZIP_NAME)"

# Attempt to prefer Homebrew LLVM 17 tools for version reporting
LLVM_PREFIX ?= $(shell brew --prefix llvm@17 2>/dev/null)
ifeq ($(LLVM_PREFIX),)
  ifneq (,$(wildcard /opt/homebrew/opt/llvm@17))
    LLVM_PREFIX := /opt/homebrew/opt/llvm@17
  else ifneq (,$(wildcard /usr/local/opt/llvm@17))
    LLVM_PREFIX := /usr/local/opt/llvm@17
  endif
endif
# Fallback to generic llvm if @17 not found
ifeq ($(LLVM_PREFIX),)
  LLVM_PREFIX := $(shell brew --prefix llvm 2>/dev/null)
  ifeq ($(LLVM_PREFIX),)
    ifneq (,$(wildcard /opt/homebrew/opt/llvm))
      LLVM_PREFIX := /opt/homebrew/opt/llvm
    else ifneq (,$(wildcard /usr/local/opt/llvm))
      LLVM_PREFIX := /usr/local/opt/llvm
    endif
  endif
endif

CLANG ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/clang,clang)
CLANGXX ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/clang++,clang++)
LLVM_CONFIG ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/llvm-config,llvm-config)
CLANG_TIDY ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/clang-tidy,clang-tidy)

# Show toolchain and system versions
version:
	@echo "==== CMake ===="
	@cmake --version | sed -n '1,3p'
	@echo
	@echo "==== Clang / LLVM ===="
	@echo "clang:   $$($(CLANG) --version 2>/dev/null | head -n1 || echo 'not found')"
	@echo "clang++: $$($(CLANGXX) --version 2>/dev/null | head -n1 || echo 'not found')"
	@printf "llvm-config: "; $(LLVM_CONFIG) --version 2>/dev/null || echo 'not found'
	@brew list --versions llvm@17 2>/dev/null || true
	@brew list --versions llvm 2>/dev/null || true
	@echo
	@echo "==== Ninja ===="
	@printf "ninja "; ninja --version 2>/dev/null || echo 'not found'
	@echo
	@echo "==== Operating System ===="
	@sw_vers 2>/dev/null || true
	@uname -mrsv
	@echo
	@echo "==== CPU ===="
	@printf "arch: "; uname -m
	@printf "model: "; sysctl -n hw.model 2>/dev/null || echo '(unavailable)'
	@printf "brand: "; sysctl -n machdep.cpu.brand_string 2>/dev/null || echo '(unavailable)'
	@printf "cores: "; sysctl -n hw.ncpu 2>/dev/null || echo '(unavailable)'
	@echo
	@echo "==== Xcode ===="
	@xcodebuild -version 2>/dev/null || echo 'Xcode not installed'

# Run CTest/GoogleTest suite
# Structured test targets (unit -> integration -> e2e)
.PHONY: unit integration e2e

unit: build
	@echo "[unit] Running unit tests..."
	@set -e; \
	UNIT_BIN="$(BUILD_DIR)/basic_compiler_unit_tests"; \
	HELLO_BIN="$(BUILD_DIR)/hello_world_tests"; \
	if [ -x "$$HELLO_BIN" ]; then echo "-- $$HELLO_BIN"; "$$HELLO_BIN"; fi; \
	if [ -x "$$UNIT_BIN" ]; then echo "-- $$UNIT_BIN"; "$$UNIT_BIN"; else echo "Unit tests binary not found: $$UNIT_BIN"; exit 2; fi

integration: unit
	@echo "[integration] Running integration tests..."
	@set -e; \
	INT_BIN="$(BUILD_DIR)/basic_compiler_integration_tests"; \
	if [ -x "$$INT_BIN" ]; then \
	  echo "-- $$INT_BIN"; \
	  "$$INT_BIN"; \
    else \
      echo "Integration tests binary not found: $$INT_BIN"; \
      exit 2; \
    fi

e2e: integration
	@echo "[e2e] Running end-to-end tests..."
	@set -e; \
	E2E_BIN="$(BUILD_DIR)/basic_compiler_e2e_tests"; \
	if [ -x "$$E2E_BIN" ]; then \
	  echo "-- $$E2E_BIN"; \
	  "$$E2E_BIN"; \
	else echo "E2E tests binary not found: $$E2E_BIN"; \
	  exit 2; \
    fi

test: e2e
	@echo "All tests completed successfully."

# Display available targets and descriptions
help:
	@echo "Available make targets:"
	@printf "  %-12s %s\n" "help"     "Show this help message"
	@echo
	@printf "  %-12s %s\n" "build"    "Configure (first run) and build all targets via CMake/Ninja"
	@printf "  %-12s %s\n" "clean"    "Delete and recreate the artifact directory 'build/'"
	@printf "  %-12s %s\n" "demo"     "Compile demos/factorial.bas to build/demos/factorial/ (bin, .ll, .bc, .asm, logs)"
	@printf "  %-12s %s\n" "e2e"      "Run end-to-end tests (depends on integration)"
	@printf "  %-12s %s\n" "integration" "Run integration tests (depends on unit)"
	@printf "  %-12s %s\n" "lint"     "Run clang-tidy on all C/C++ sources"
	@printf "  %-12s %s\n" "test"     "Run unit, integration, and e2e tests"
	@printf "  %-12s %s\n" "tree"     "Print repository tree (requires 'tree')"
	@printf "  %-12s %s\n" "unit"     "Run unit tests"
	@printf "  %-12s %s\n" "version"  "Show versions for CMake, Clang/LLVM, Ninja, OS, CPU, Xcode"
	@printf "  %-12s %s\n" "zip"      "Archive repository into $(ZIP_NAME), excluding build/ and VCS/IDE files"
	@echo
	@echo "Overridable variables (current values):"
	@printf "  %-14s = %s\n" "BUILD_DIR" "$(BUILD_DIR)"
	@printf "  %-14s = %s\n" "GENERATOR" "$(GENERATOR)"
	@printf "  %-14s = %s\n" "TOOLCHAIN" "$(TOOLCHAIN)"
	@printf "  %-14s = %s\n" "CONFIG"    "$(CONFIG)"
	@printf "  %-14s = %s\n" "NUM_CPUS"  "$(NUM_CPUS)"
	@printf "  %-14s = %s\n" "ZIP_NAME"  "$(ZIP_NAME)"
	@printf "  %-14s = %s\n" "TREE_ROOT" "$(TREE_ROOT)"
	@printf "  %-14s = %s\n" "TREE_IGNORES" "$(TREE_IGNORES)"
	@echo
	@echo "Examples:"
	@echo "  make build CONFIG=Release"
	@echo "  make lint BUILD_DIR=build/cmake-build-debug"
	@echo "  make tree TREE_ROOT=src"

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

# Lint all C/C++ sources using clang-tidy
LINT_PATTERNS ?= -name '*.c' -o -name '*.cc' -o -name '*.cxx' -o -name '*.cpp'
LINT_PRUNE ?= \( -path './.git' -o -path './build' -o -path './build-*' -o -path './cmake-build-*' -o -path './out' -o -path './.idea' -o -path './CMakeFiles' -o -path './Testing' \) -prune
lint:
	@command -v $(CLANG_TIDY) >/dev/null 2>&1 || { echo "clang-tidy not found; skipping lint."; exit 0; }
	@echo "Running clang-tidy across repository sources..."
	@set -e; \
	SDK=$$(xcrun --show-sdk-path 2>/dev/null || true); \
	SDK_ARGS=""; \
	if [ -n "$$SDK" ]; then SDK_ARGS="--extra-arg=-isysroot --extra-arg=$$SDK"; fi; \
	if [ -f "$(BUILD_DIR)/compile_commands.json" ]; then \
	  FILES=$$(sed -n 's/.*"file"[[:space:]]*:[[:space:]]*"\(.*\)".*/\1/p' "$(BUILD_DIR)/compile_commands.json" | sort -u); \
	else \
	  FILES=$$(find . $(LINT_PRUNE) -o -type f \( $(LINT_PATTERNS) \) -print); \
	fi; \
	if [ -z "$$FILES" ]; then echo "No C/C++ source files found to lint."; exit 0; fi; \
		for f in $$FILES; do \
		  echo "-- $$f"; \
		  "$(CLANG_TIDY)" -p "$(BUILD_DIR)" -warnings-as-errors='*' $$SDK_ARGS "$$f" || exit $$?; \
		done; \
	echo "clang-tidy completed."

# Coverage: instrument build, run unit tests, and report codegen coverage
.PHONY: coverage
# Variables:
#  - COVERAGE_MIN: minimum percent for pass (default 80)
#  - COVERAGE_SCOPE: path substring to aggregate (default src/basic_compiler/codegenerator/)
#  - COVERAGE_METRIC: lines|regions|both (default both)
COVERAGE_MIN ?= 80
COVERAGE_SCOPE ?= src/basic_compiler/codegenerator/
COVERAGE_METRIC ?= both
coverage:
	@echo "[coverage] Configuring with CODE_COVERAGE=ON..."
	@$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG) -DCMAKE_BUILD_TYPE=$(CONFIG) -DCODE_COVERAGE=ON
	@echo "[coverage] Building tests..."
	@$(CMAKE) --build $(BUILD_DIR) -v -- -j$(NUM_CPUS)
	@echo "[coverage] Running unit tests with profiling..."
	@set -e; \
	  OUT="$(BUILD_DIR)/coverage"; mkdir -p "$$OUT"; \
	  UNIT_BIN="$(BUILD_DIR)/basic_compiler_unit_tests"; \
	  if [ ! -x "$$UNIT_BIN" ]; then echo "Unit tests not found: $$UNIT_BIN"; exit 2; fi; \
	  LLVM_PROFILE_FILE="$$OUT/unit-%p.profraw" "$$UNIT_BIN" >/dev/null; \
	  echo "[coverage] Merging profiles..."; \
	  LLVM_PROFDATA=$(LLVM_PREFIX)/bin/llvm-profdata; \
	  if [ ! -x "$$LLVM_PROFDATA" ]; then LLVM_PROFDATA=llvm-profdata; fi; \
	  $$LLVM_PROFDATA merge -sparse "$$OUT"/*.profraw -o "$$OUT/coverage.profdata"; \
	  echo "[coverage] Generating report..."; \
	  LLVM_COV=$(LLVM_PREFIX)/bin/llvm-cov; \
	  if [ ! -x "$$LLVM_COV" ]; then LLVM_COV=llvm-cov; fi; \
	  $$LLVM_COV report "$$UNIT_BIN" -instr-profile="$$OUT/coverage.profdata" -use-color=false > "$$OUT/report.txt"; \
	  echo "[coverage] Aggregating scope: $(COVERAGE_SCOPE)"; \
	  SCOPE="$(COVERAGE_SCOPE)"; \
	  grep "$$SCOPE" "$$OUT/report.txt" | awk '{ reg+=$$2; miss+=$$3; line+=$$8; lmiss+=$$9 } END { \
	    if (reg>0) printf("Regions %d/%d %.0f\n", reg-miss, reg, (reg-miss)*100/reg); \
	    if (line>0) printf("Lines %d/%d %.0f\n", line-lmiss, line, (line-lmiss)*100/line); \
	  }' > "$$OUT/scope.txt"; \
	  echo "[coverage] Scope summary:"; cat "$$OUT/scope.txt" | sed 's/^/  /'; \
	  LINES=$$(awk '/^Lines/{print $$3}' "$$OUT/scope.txt"); \
	  REGS=$$(awk '/^Regions/{print $$3}' "$$OUT/scope.txt"); \
	  FAIL=0; \
	  case "$(COVERAGE_METRIC)" in \
	    lines)   VAL=$$LINES;; \
	    regions) VAL=$$REGS;; \
	    both)    VAL=$$LINES; if [ -n "$$REGS" ] && [ $${REGS:-0} -lt $(COVERAGE_MIN) ]; then FAIL=1; fi;; \
	    *)       VAL=$$LINES;; \
	  esac; \
	  if [ -z "$$VAL" ]; then echo "Could not parse coverage values for scope $(COVERAGE_SCOPE)."; exit 4; fi; \
	  if [ $${VAL:-0} -lt $(COVERAGE_MIN) ]; then FAIL=1; fi; \
	  if [ $$FAIL -ne 0 ]; then echo "Coverage below threshold $(COVERAGE_MIN)% (lines=$$LINES, regions=$$REGS)."; exit 3; fi; \
	  echo "[coverage] OK. Detailed report: $$OUT/report.txt"
