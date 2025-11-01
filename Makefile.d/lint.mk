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
lint: build
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
