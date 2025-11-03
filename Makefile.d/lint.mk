# File: Makefile.d/lint.mk
#
# Purpose: Invoke CMake linter target (clang-tidy) and run extra shell checks.
#
# Targets:
#  - lint: builds CMake 'lint' target with Ninja parallelism, then runs script checks.

lint: configure
	@echo "Invoking CMake linter target (parallel -j$(NUM_CPUS))..."
	@$(CMAKE) --build $(BUILD_DIR) --target lint -v -- -j$(NUM_CPUS)
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

