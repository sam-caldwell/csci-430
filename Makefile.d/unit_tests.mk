# File: Makefile.d/unit_tests.mk
#
# Purpose: Run unit tests via CTest in parallel.
#
# Targets:
#  - unit: executes CTest tests with label 'unit' using $(NUM_CPUS) jobs.
unit: build
	@echo "[unit] Running unit tests (ctest, -j$(NUM_CPUS))..."
	@set -e; \
	  CTEST_PARALLEL_LEVEL="$(NUM_CPUS)" \
	  ctest --test-dir "$(BUILD_DIR)" --output-on-failure -L unit -j "$(NUM_CPUS)"
