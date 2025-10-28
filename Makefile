# File: Makefile
# (c) 2025 Sam Caldwell.  All Rights Reserved.
# Purpose: Convenience wrapper for configure/build and environment reporting.
#          Provides 'build', 'clean', and 'version' targets for developers.
.PHONY: clean build configure version tree zip lint test help
.DEFAULT_GOAL := help

# Removes the artifact directory and recreates it
clean:
	@echo "Cleaning build/ directory..."
	@rm -rf build
	@mkdir -p build
	@echo "Recreated build/"

# Configuration
CMAKE ?= cmake
GENERATOR ?= Ninja
BUILD_DIR ?= cmake-build-debug
TOOLCHAIN ?= cmake/Toolchain-HomebrewLLVM.cmake
CONFIG ?= Debug

configure:
	@if [ ! -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN) -DCMAKE_BUILD_TYPE=$(CONFIG); \
	else \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR); \
	fi

# Build all targets via CMake/Ninja
build: configure
	@$(CMAKE) --build $(BUILD_DIR) -v

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
test: build
	@ctest --test-dir "$(BUILD_DIR)" --output-on-failure -C "$(CONFIG)"

# Display available targets and descriptions
help:
	@echo "Available make targets:"
	@printf "  %-12s %s\n" "help"     "Show this help message"
	@printf "  %-12s %s\n" "build"    "Configure (first run) and build all targets via CMake/Ninja"
	@printf "  %-12s %s\n" "configure" "Configure CMake in $(BUILD_DIR) using generator $(GENERATOR)"
	@printf "  %-12s %s\n" "test"     "Run unit/integration tests (CTest/GoogleTest)"
	@printf "  %-12s %s\n" "lint"     "Run clang-tidy on all C/C++ sources"
	@printf "  %-12s %s\n" "version"  "Show versions for CMake, Clang/LLVM, Ninja, OS, CPU, Xcode"
	@printf "  %-12s %s\n" "tree"     "Print repository tree (requires 'tree')"
	@printf "  %-12s %s\n" "zip"      "Archive repository into $(ZIP_NAME), excluding build/ and VCS/IDE files"
	@printf "  %-12s %s\n" "clean"    "Delete and recreate the artifact directory 'build/'"
	@echo
	@echo "Overridable variables (current values):"
	@printf "  %-14s = %s\n" "BUILD_DIR" "$(BUILD_DIR)"
	@printf "  %-14s = %s\n" "GENERATOR" "$(GENERATOR)"
	@printf "  %-14s = %s\n" "TOOLCHAIN" "$(TOOLCHAIN)"
	@printf "  %-14s = %s\n" "CONFIG"    "$(CONFIG)"
	@printf "  %-14s = %s\n" "ZIP_NAME"  "$(ZIP_NAME)"
	@printf "  %-14s = %s\n" "TREE_ROOT" "$(TREE_ROOT)"
	@printf "  %-14s = %s\n" "TREE_IGNORES" "$(TREE_IGNORES)"
	@echo
	@echo "Examples:"
	@echo "  make build CONFIG=Release"
	@echo "  make lint BUILD_DIR=cmake-build-debug"
	@echo "  make tree TREE_ROOT=src"

# Lint all C/C++ sources using clang-tidy
LINT_PATTERNS ?= -name '*.c' -o -name '*.cc' -o -name '*.cxx' -o -name '*.cpp'
LINT_PRUNE ?= \( -path './.git' -o -path './build' -o -path './build-*' -o -path './cmake-build-*' -o -path './out' -o -path './.idea' -o -path './CMakeFiles' -o -path './Testing' \) -prune
lint: configure
	@command -v $(CLANG_TIDY) >/dev/null 2>&1 || { echo "clang-tidy not found. Install via: brew install llvm"; exit 1; }
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
	  "$(CLANG_TIDY)" -p "$(BUILD_DIR)" $$SDK_ARGS "$$f" || exit $$?; \
	done; \
	echo "clang-tidy completed."
