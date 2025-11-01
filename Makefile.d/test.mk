
unit: build
	@echo "[unit] Running unit tests..."
	@set -e; \
	UNIT_BIN="$(BUILD_DIR)/basic_compiler_unit_tests"; \
	HELLO_BIN="$(BUILD_DIR)/hello_world_tests"; \
	if [ -x "$$HELLO_BIN" ]; then echo "-- $$HELLO_BIN"; "$$HELLO_BIN"; fi; \
	if [ -x "$$UNIT_BIN" ]; then echo "-- $$UNIT_BIN"; "$$UNIT_BIN"; else echo "Unit tests binary not found: $$UNIT_BIN"; exit 2; fi

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

test: e2e
	@echo "All tests completed successfully."
