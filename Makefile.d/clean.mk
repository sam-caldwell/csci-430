# File: Makefile.d/clean.mk
#
# Purpose: Remove build artifacts and recreate a fresh build/ directory.
#
# Targets:
#  - clean: Deletes build, cmake-build-*; recreates build/ for convenience.
#
# Notes:
#  - Safe to run repeatedly; ignores missing paths.
#
# Removes the artifact directory and recreates it
clean:
	@echo "Cleaning build/ directory..."
	@rm -rf build build-basic cmake-build-* || true
	@mkdir -p build
	@echo "Recreated build/"
