# File: Makefile.d/lint.mk
#
# Purpose: Invoke CMake linter target (clang-tidy) and run extra shell checks.
#
# Targets:
#  - lint: builds CMake 'lint' target with Ninja parallelism, then runs script checks.

lint: configure
	@echo "Checking test docstrings and TEST constraints..."
	@$(CMAKE) --build $(BUILD_DIR) --target check_test_docstrings -v -- -j$(NUM_CPUS)
	@echo "Checking one-function-per-file convention..."
	@$(CMAKE) --build $(BUILD_DIR) --target check_one_function_per_file -v -- -j$(NUM_CPUS)
	@echo "Invoking CMake linter target (parallel -j$(NUM_CPUS))..."
	@$(CMAKE) --build $(BUILD_DIR) --target lint -v -- -j$(NUM_CPUS)
