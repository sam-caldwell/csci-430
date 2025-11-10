# File: Makefile.d/lint.mk
#
# Purpose: Invoke CMake linter target (clang-tidy) and run extra shell checks.
#
# Targets:
#  - lint: builds CMake 'lint' target with Ninja parallelism, then runs script checks.

lint: configure
	@echo "Running aggregate lint (docstrings, checks, clang-tidy)..."
	@$(CMAKE) --build $(BUILD_DIR) --target lint -v -- -j$(NUM_CPUS)
	@echo "lint: ok"
