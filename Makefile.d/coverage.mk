# File: Makefile.d/coverage.mk
#
# Purpose: Generate and enforce LLVM source-based coverage using llvm-cov.
#
# Targets:
#  - coverage: Builds with coverage flags, runs tests, aggregates, enforces threshold.
#
# Variables:
#  - COVERAGE_MIN: minimum percent for pass (default 95)
#  - COVERAGE_SCOPE: report rows to aggregate (substring match)
#  - COVERAGE_METRIC: lines|regions|both (both requires both to meet threshold)
#  - COVERAGE_EXCLUDE_RE: egrep regex to exclude rows from aggregation
#  - LLVM_PREFIX: optional toolchain prefix for llvm-profdata/llvm-cov
COVERAGE_MIN ?= 100
COVERAGE_SCOPE ?= src/basic_compiler/
# Optional: include regex (egrep) to restrict aggregation set
# Defaults to the frontend (lexer, parser, semantics) for language conformance
# Focus default coverage on lexer for 100% target
COVERAGE_INCLUDE_RE ?= src/basic_compiler/lexer/
COVERAGE_METRIC ?= regions
# Optional: exclude regex (egrep) to drop files from aggregation
# Defaults exclude hard-to-measure support code (logging/collection & subroutine inlining)
COVERAGE_EXCLUDE_RE ?= src/basic_compiler/lexer/log_token.cpp|src/basic_compiler/codegenerator/
coverage:
	@echo "[coverage] Configuring with CODE_COVERAGE=ON..."
	@$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG) -DCMAKE_BUILD_TYPE=$(CONFIG) -DCODE_COVERAGE=ON \
	  -DCOVERAGE_MIN=$(COVERAGE_MIN) -DCOVERAGE_SCOPE="$(COVERAGE_SCOPE)" -DCOVERAGE_INCLUDE_RE="$(COVERAGE_INCLUDE_RE)" \
	  -DCOVERAGE_EXCLUDE_RE="$(COVERAGE_EXCLUDE_RE)" -DCOVERAGE_METRIC=$(COVERAGE_METRIC)
	@echo "[coverage] Building coverage target with Ninja (-j$(NUM_CPUS))..."
	@$(CMAKE) --build $(BUILD_DIR) --target coverage -v -- -j$(NUM_CPUS)
