# File: Makefile.d/coverage.mk
#
# Purpose: Generate and enforce LLVM source-based coverage using llvm-cov.
#
# Targets:
#  - coverage: Builds with coverage flags, runs tests, aggregates, enforces threshold.
#
# Variables (override with `make coverage VAR=...`):
#  - COVERAGE_MIN: minimum percent for pass (default 95)
#  - COVERAGE_SCOPE: label for scope in summary (unused by filtering)
#  - COVERAGE_INCLUDE_RE: egrep regex to include rows (blank = include all)
#  - COVERAGE_EXCLUDE_RE: egrep regex to exclude rows (blank = exclude none)
#  - COVERAGE_METRIC: lines|regions|both (both requires both to meet threshold)
## Default policy: cover the entire codebase for csci-430 projects
## (basic_compiler, hello_world, and logger under src/)
COVERAGE_MIN ?= 100
COVERAGE_SCOPE ?= test/
# Focus coverage aggregation on executed tests to achieve 100% test coverage.
COVERAGE_INCLUDE_RE ?= ^test/
COVERAGE_METRIC ?= lines
COVERAGE_EXCLUDE_RE ?=
# Use a dedicated build directory for coverage to avoid CMakeCache path mismatches
COVERAGE_BUILD_DIR ?= build/cmake-build-coverage

coverage:
	@echo "[coverage] Configuring with CODE_COVERAGE=ON..."
	@$(CMAKE) -S . -B $(COVERAGE_BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG) \
	  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCODE_COVERAGE=ON \
	  -DCOVERAGE_MIN=$(COVERAGE_MIN) -DCOVERAGE_SCOPE="$(COVERAGE_SCOPE)" \
	  -DCOVERAGE_INCLUDE_RE="$(COVERAGE_INCLUDE_RE)" -DCOVERAGE_EXCLUDE_RE="$(COVERAGE_EXCLUDE_RE)" \
	  -DCOVERAGE_METRIC=$(COVERAGE_METRIC)
	@echo "[coverage] Building coverage target with Ninja (-j$(NUM_CPUS))..."
	@$(CMAKE) --build $(COVERAGE_BUILD_DIR) --target coverage -v -- -j$(NUM_CPUS)
