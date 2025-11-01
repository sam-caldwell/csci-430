# File: Makefile.d/e2e_tests.mk
#
# Purpose: Run end-to-end tests after integration tests.
#
# Targets:
#  - e2e: Executes the basic_compiler_e2e_tests binary if present.
e2e: integration
	@echo "[e2e] Running end-to-end tests..."
	@set -e; \
	E2E_BIN="$(BUILD_DIR)/basic_compiler_e2e_tests"; \
	if [ -x "$$E2E_BIN" ]; then \
	  echo "-- $$E2E_BIN"; \
	  "$$E2E_BIN"; \
	else echo "E2E tests binary not found: $$E2E_BIN"; \
	  exit 2; \
    fi
