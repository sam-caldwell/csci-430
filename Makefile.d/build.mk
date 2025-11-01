# File: Makefile.d/build.mk
#
# Purpose: Configure (if needed) and build all targets via CMake/Ninja.
#
# Targets:
#  - build: Generate (on first run) and build with $(NUM_CPUS) jobs.
#
# Variables:
#  - CMAKE, GENERATOR, TOOLCHAIN_FLAG, CONFIG, BUILD_DIR, NUM_CPUS
#
# Notes:
#  - Auto-configures when no CMakeCache.txt is present in $(BUILD_DIR).
#
# Build all targets via CMake/Ninja (auto-configure if needed)
build:
	@if [ ! -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG) -DCMAKE_BUILD_TYPE=$(CONFIG); \
	else \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG); \
	fi
	@echo "Building with $(NUM_CPUS) parallel jobs..."
	@$(CMAKE) --build $(BUILD_DIR) -v -- -j$(NUM_CPUS)
