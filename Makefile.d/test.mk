# File: Makefile.d/test.mk
#
# Purpose: Aggregate test target that runs all test tiers.
#
# Targets:
#  - test: depends on e2e (which depends on integration -> unit)
test: e2e
	@echo "All tests completed successfully."
