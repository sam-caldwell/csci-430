# File: Makefile.d/lint.mk
#
# Purpose: Lint all C/C++ sources using clang-tidy.
#
# Targets:
#  - lint: Runs clang-tidy with compile_commands.json if available.
#
# Variables:
#  - LINT_PATTERNS, LINT_PRUNE, CLANG_TIDY, BUILD_DIR
#
# Lint all C/C++ sources using clang-tidy
LINT_PATTERNS ?= -name '*.c' -o -name '*.cc' -o -name '*.cxx' -o -name '*.cpp'
LINT_PRUNE ?= \( -path './.git' -o -path './build' -o -path './build-*' -o -path './cmake-build-*' -o -path './out' -o -path './.idea' -o -path './CMakeFiles' -o -path './Testing' \) -prune
lint: configure
	@command -v $(CLANG_TIDY) >/dev/null 2>&1 || { echo "clang-tidy not found; skipping lint."; exit 0; }
	@echo "Running clang-tidy across repository sources..."
	@set -e; \
	  TIDY_CHECKS="${TIDY_CHECKS:--*,clang-diagnostic-unused-*,bugprone-unused-return-value,clang-analyzer-deadcode.DeadStores,misc-unused-parameters}"; \
	  echo "clang-tidy checks: $$TIDY_CHECKS"; \
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
		"$(CLANG_TIDY)" -p "$(BUILD_DIR)" -checks="$$TIDY_CHECKS" -warnings-as-errors='*' $$SDK_ARGS "$$f" || exit $$?; \
	  done; \
	  echo "clang-tidy completed."
	@echo "Running one-function-per-file static check..."
	@bash scripts/check_one_function_per_file.sh
	@echo "Running shellcheck on scripts/*.sh (if available)..."
	@set -e; \
	if command -v shellcheck >/dev/null 2>&1; then \
	  SCRIPTS=$$(find scripts -type f -name '*.sh' 2>/dev/null | sort); \
	  if [ -z "$$SCRIPTS" ]; then \
		echo "No shell scripts found under scripts/."; \
	  else \
		echo "shellcheck files:"; echo "$$SCRIPTS" | sed 's/^/ - /'; \
		shellcheck -x $$SCRIPTS; \
		echo "shellcheck completed."; \
	  fi; \
	else \
	  echo "shellcheck not found; skipping shell script lint."; \
	fi
