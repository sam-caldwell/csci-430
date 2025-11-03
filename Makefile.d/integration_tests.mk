# File: Makefile.d/integration_tests.mk
#
# Purpose: Run integration tests via CTest in parallel (after unit).
#
# Targets:
#  - integration: Executes CTest tests with label 'integration' using $(NUM_CPUS) jobs.
integration: unit
	@echo "[integration] Running integration tests (ctest, -j$(NUM_CPUS))..."
	@set -e; \
	  CTEST_PARALLEL_LEVEL="$(NUM_CPUS)" \
	  ctest --test-dir "$(BUILD_DIR)" --output-on-failure -L integration -j "$(NUM_CPUS)"
