# File: Makefile.d/lint.mk
#
# Purpose: Provide two lint targets:
#  - lint: fast, parallel clang-tidy across compile_commands.json entries.
#  - linter/slow: existing CMake-driven fail-fast sequential clang-tidy.
#
# Notes:
#  - 'lint' parses $(BUILD_DIR)/compile_commands.json to discover files and
#    runs clang-tidy in parallel via xargs with $(NUM_CPUS) workers.
#  - 'linter/slow' retains the previous behavior (CMake script, fail-fast).

# Fast, parallel clang-tidy over all TU entries in compile_commands.json
lint: configure
	@echo "Running fast clang-tidy (parallel; -j$(NUM_CPUS))..."
	@set -e; \
	  if ! command -v $(CLANG_TIDY) >/dev/null 2>&1; then \
	    echo "clang-tidy not found; install it or set CLANG_TIDY=<path>"; exit 1; \
	  fi; \
	  if [ ! -f "$(BUILD_DIR)/compile_commands.json" ]; then \
	    echo "compile_commands.json not found in $(BUILD_DIR). Run 'make configure' or 'make build'."; \
	    exit 1; \
	  fi; \
	  SYSROOT=""; \
	  if command -v xcrun >/dev/null 2>&1; then \
	    SYSROOT="`xcrun --show-sdk-path 2>/dev/null || true`"; \
	  fi; \
	  FILES="$$( \
	    if command -v rg >/dev/null 2>&1; then \
	      rg -o --no-line-number '"file"\s*:\s*"([^"]+)"' -r '$$1' "$(BUILD_DIR)/compile_commands.json"; \
	    else \
	      sed -n 's/.*"file"[[:space:]]*:[[:space:]]*"\([^\"]*\)".*/\1/p' "$(BUILD_DIR)/compile_commands.json"; \
	    fi | awk '!seen[$$0]++' \
	  )"; \
	  if [ -z "$$FILES" ]; then \
	    echo "No files found in compile_commands.json"; exit 0; \
	  fi; \
	  echo "Invoking clang-tidy on $$(( $$(printf '%s\n' "$$FILES" | wc -l | tr -d ' ') )) files..."; \
	  printf '%s\n' "$$FILES" | xargs -P "$(NUM_CPUS)" -n 1 $(CLANG_TIDY) \
	    -p "$(BUILD_DIR)" -warnings-as-errors=* -quiet \
	    $$([ -n "$$SYSROOT" ] && printf -- " --extra-arg=-isysroot --extra-arg=%s" "$$SYSROOT")
	@echo "lint: ok"

# Slow, fail-fast clang-tidy via the existing CMake script
linter/slow: configure
	@echo "Running slow clang-tidy (CMake fail-fast script)..."
	@$(CMAKE) --build $(BUILD_DIR) --target lint -v -- -j$(NUM_CPUS)
	@echo "linter/slow: ok"

# Slow linter scoped to a specific subdirectory under the repo root.
# Usage: make linter/<relative/path>
linter/%: configure
	@echo "Running slow clang-tidy for path '$*'..."
	@set -e; \
	  if [ ! -f "$(BUILD_DIR)/compile_commands.json" ]; then \
	    echo "compile_commands.json not found in $(BUILD_DIR). Run 'make configure' or 'make build'."; \
	    exit 1; \
	  fi; \
	  cmake \
	    -DCLANG_TIDY_EXE="$(CLANG_TIDY)" \
	    -DBINARY_DIR="$(BUILD_DIR)" \
	    -DROOT_DIR="$(CURDIR)" \
	    -DFILTER_DIR="$*" \
	    -P cmake/run_clang_tidy_failfast.cmake
	@echo "linter/$*: ok"
