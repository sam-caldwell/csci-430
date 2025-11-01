# File: Makefile.d/unit_tests.mk
#
# Purpose: Run unit tests (and hello_world tests) from the build tree.
#
# Targets:
#  - unit: executes hello_world_tests (if present) and basic_compiler_unit_tests
unit: build
	@echo "[unit] Running unit tests..."
	@set -e; \
	UNIT_BIN="$(BUILD_DIR)/basic_compiler_unit_tests"; \
	HELLO_BIN="$(BUILD_DIR)/hello_world_tests"; \
	if [ -x "$$HELLO_BIN" ]; then echo "-- $$HELLO_BIN"; "$$HELLO_BIN"; fi; \
	if [ -x "$$UNIT_BIN" ]; then echo "-- $$UNIT_BIN"; "$$UNIT_BIN"; else echo "Unit tests binary not found: $$UNIT_BIN"; exit 2; fi
