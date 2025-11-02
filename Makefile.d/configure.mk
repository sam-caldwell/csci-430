# File: Makefile.d/build.mk
#
# Purpose: Configure CMake only (no build)
configure:
	@mkdir -p "$(BUILD_DIR)"
	@echo "Configuring with generator: $(GENERATOR) (build dir=$(BUILD_DIR))";
	@if [ ! -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG) -DCMAKE_BUILD_TYPE=$(CONFIG) -DBUILD_TESTING=ON; \
	else \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG) -DBUILD_TESTING=ON; \
	fi
	@echo "configure complete: $(BUILD_DIR)"
