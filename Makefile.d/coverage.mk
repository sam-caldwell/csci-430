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
COVERAGE_MIN ?= 95
COVERAGE_SCOPE ?= src/basic_compiler/
COVERAGE_METRIC ?= both
# Optional: exclude regex (egrep) to drop files from aggregation
# Defaults exclude hard-to-measure support code (logging/collection & subroutine inlining)
COVERAGE_EXCLUDE_RE ?= src/basic_compiler/lexer/log_token.cpp|src/basic_compiler/codegenerator/
coverage:
	@echo "[coverage] Configuring with CODE_COVERAGE=ON..."
	@$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG) -DCMAKE_BUILD_TYPE=$(CONFIG) -DCODE_COVERAGE=ON
	@echo "[coverage] Building tests..."
	@$(CMAKE) --build $(BUILD_DIR) -v -- -j$(NUM_CPUS)
	@echo "[coverage] Running unit tests with profiling..."
	@set -e; \
	  OUT="$(BUILD_DIR)/coverage"; mkdir -p "$$OUT"; \
	  UNIT_BIN="$(BUILD_DIR)/basic_compiler_unit_tests"; \
	  if [ ! -x "$$UNIT_BIN" ]; then echo "Unit tests not found: $$UNIT_BIN"; exit 2; fi; \
	  LLVM_PROFILE_FILE="$$OUT/unit-%p.profraw" "$$UNIT_BIN" >/dev/null; \
	  echo "[coverage] Merging profiles..."; \
	  LLVM_PROFDATA=$(LLVM_PREFIX)/bin/llvm-profdata; \
	  if [ ! -x "$$LLVM_PROFDATA" ]; then LLVM_PROFDATA=llvm-profdata; fi; \
	  $$LLVM_PROFDATA merge -sparse "$$OUT"/*.profraw -o "$$OUT/coverage.profdata"; \
	  echo "[coverage] Generating report..."; \
	  LLVM_COV=$(LLVM_PREFIX)/bin/llvm-cov; \
	  if [ ! -x "$$LLVM_COV" ]; then LLVM_COV=llvm-cov; fi; \
	  $$LLVM_COV report "$$UNIT_BIN" -instr-profile="$$OUT/coverage.profdata" -use-color=false > "$$OUT/report.txt"; \
	  echo "[coverage] Aggregating scope: $(COVERAGE_SCOPE)"; \
	  SCOPE="$(COVERAGE_SCOPE)"; \
	  awk_input="$$OUT/report.txt"; \
	  if [ -n "$(COVERAGE_EXCLUDE_RE)" ]; then \
	    echo "[coverage] Excluding: $(COVERAGE_EXCLUDE_RE)"; \
	    grep "$$SCOPE" "$$OUT/report.txt" | egrep -v "$(COVERAGE_EXCLUDE_RE)" > "$$OUT/filtered.txt"; \
	    awk_input="$$OUT/filtered.txt"; \
	  fi; \
	  awk '{ reg+=$$2; miss+=$$3; line+=$$8; lmiss+=$$9 } END { \
	    if (reg>0) printf("Regions %d/%d %.0f\n", reg-miss, reg, (reg-miss)*100/reg); \
	    if (line>0) printf("Lines %d/%d %.0f\n", line-lmiss, line, (line-lmiss)*100/line); \
	  }' "$$awk_input" > "$$OUT/scope.txt"; \
	  echo "[coverage] Scope summary:"; cat "$$OUT/scope.txt" | sed 's/^/  /'; \
	  LINES=$$(awk '/^Lines/{print $$3}' "$$OUT/scope.txt"); \
	  REGS=$$(awk '/^Regions/{print $$3}' "$$OUT/scope.txt"); \
	  FAIL=0; \
	  case "$(COVERAGE_METRIC)" in \
	    lines)   VAL=$$LINES;; \
	    regions) VAL=$$REGS;; \
	    both)    VAL=$$LINES; if [ -n "$$REGS" ] && [ $${REGS:-0} -lt $(COVERAGE_MIN) ]; then FAIL=1; fi;; \
	    *)       VAL=$$LINES;; \
	  esac; \
	  if [ -z "$$VAL" ]; then echo "Could not parse coverage values for scope $(COVERAGE_SCOPE)."; exit 4; fi; \
	  if [ $${VAL:-0} -lt $(COVERAGE_MIN) ]; then FAIL=1; fi; \
	  if [ $$FAIL -ne 0 ]; then echo "Coverage below threshold $(COVERAGE_MIN)% (lines=$$LINES, regions=$$REGS)."; exit 3; fi; \
	  echo "[coverage] OK. Detailed report: $$OUT/report.txt"
