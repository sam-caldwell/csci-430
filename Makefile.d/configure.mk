# File: Makefile.d/build.mk
#
# Purpose: Configure CMake only (no build)
configure:
	@mkdir -p "$(BUILD_DIR)"
	@echo "Configuring with generator: $(GENERATOR) (build dir=$(BUILD_DIR))";
	@if [ ! -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG) -DCMAKE_BUILD_TYPE=$(CONFIG); \
	else \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG); \
	fi
	@echo "configure complete: $(BUILD_DIR)"