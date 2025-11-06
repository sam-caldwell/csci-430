# File: Makefile.d/e2e_tests.mk
#
# Purpose: Run end-to-end tests via CTest, with safeguards to avoid
#          exhausting CPU or memory.
#
# Targets:
#  - e2e: Executes CTest tests with label 'e2e' using capped parallelism
#         and per-test timeout. Output capture is further limited by the
#         e2e helper (see test/basic_compiler/e2e/run_command.h).

# Allow overriding from the environment/CLI, but cap by default.
# Default to min(NUM_CPUS, 2) to reduce peak load when E2E compiles and runs
# many programs concurrently.
E2E_JOBS ?= $(shell J=$(NUM_CPUS); if [ "$$J" -gt 2 ]; then echo 2; else echo $$J; fi)

# Per-test timeout (seconds). Can be overridden: make e2e E2E_TIMEOUT=300
E2E_TIMEOUT ?= 180

# Maximum bytes captured from child process stdout in e2e helpers.
# Can be overridden: make e2e E2E_MAX_OUTPUT=262144
E2E_MAX_OUTPUT ?= 131072

e2e: integration
	@echo "[e2e] Running end-to-end tests (ctest, -j$(E2E_JOBS), --timeout $(E2E_TIMEOUT))..."
	@set -e; \
	  E2E_MAX_OUTPUT="$(E2E_MAX_OUTPUT)" \
	  CTEST_PARALLEL_LEVEL="$(E2E_JOBS)" \
	  ctest --test-dir "$(BUILD_DIR)" --output-on-failure -L e2e -j "$(E2E_JOBS)" --timeout "$(E2E_TIMEOUT)"
