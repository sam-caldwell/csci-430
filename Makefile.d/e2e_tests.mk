# File: Makefile.d/e2e_tests.mk
#
# Purpose: Run end-to-end tests via CTest in parallel (after integration).
#
# Targets:
#  - e2e: Executes CTest tests with label 'e2e' using $(NUM_CPUS) jobs.
e2e: integration
	@echo "[e2e] Running end-to-end tests (ctest, -j$(NUM_CPUS))..."
	@set -e; \
	  CTEST_PARALLEL_LEVEL="$(NUM_CPUS)" \
	  ctest --test-dir "$(BUILD_DIR)" --output-on-failure -L e2e -j "$(NUM_CPUS)"
