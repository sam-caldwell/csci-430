# File: Makefile.d/zip.mk
# Purpose: Archive repository into a zip (excluding build and VCS/IDE artifacts)

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

.PHONY: zip
zip:
	@command -v zip >/dev/null 2>&1 || { echo "zip not found. Install with: brew install zip || sudo apt-get install zip"; exit 1; }
	@echo "Creating $(ZIP_NAME) ..."
	@rm -f "$(ZIP_NAME)"
	@zip -r -q "$(ZIP_NAME)" . -x $(ZIP_EXCLUDES)
	@echo "Wrote $(ZIP_NAME)"
