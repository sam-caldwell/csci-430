# File: Makefile.d/integration_tests.mk
#
# Purpose: Run integration tests after unit tests.
#
# Targets:
#  - integration: Executes the basic_compiler_integration_tests binary.
integration: unit
	@echo "[integration] Running integration tests..."
	@set -e; \
	INT_BIN="$(BUILD_DIR)/basic_compiler_integration_tests"; \
	if [ -x "$$INT_BIN" ]; then \
	  echo "-- $$INT_BIN"; \
	  "$$INT_BIN"; \
    else \
      echo "Integration tests binary not found: $$INT_BIN"; \
      exit 2; \
    fi
