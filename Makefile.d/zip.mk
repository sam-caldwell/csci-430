# File: Makefile.d/zip.mk
#
# Purpose: Create a zip archive of the repository, excluding build and tooling outputs.
#
# Targets:
#  - zip: writes $(ZIP_NAME) with standard exclusions.
#
# Variables:
#  - ZIP_BASE, ZIP_NAME, ZIP_EXCLUDES
#
# Zip the repository (excluding build and VCS/IDE artifacts)
ZIP_BASE := $(notdir $(CURDIR))
ZIP_NAME ?= $(ZIP_BASE).zip
ZIP_EXCLUDES ?= \
  ".git/*" \
  ".zip/*" \
  "build/*" \
  "build-*/*" \
  "cmake-build-*/*" \
  "out/*" \
  ".idea/*" \
  "CMakeFiles/*" \
  "Testing/*" \
  "$(ZIP_NAME)" \
  "*.o" "*.obj" "*.a" "*.so" "*.dylib" "*.dll"

zip:
	@command -v zip >/dev/null 2>&1 || { echo "zip not found. Install with: brew install zip"; exit 1; }
	@echo "Creating $(ZIP_NAME) ..."
	@rm -f "$(ZIP_NAME)"
	@zip -r -q "$(ZIP_NAME)" . -x $(ZIP_EXCLUDES)
	@echo "Wrote $(ZIP_NAME)"
