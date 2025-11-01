# Build all targets via CMake/Ninja (auto-configure if needed)
build:
	@if [ ! -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG) -DCMAKE_BUILD_TYPE=$(CONFIG); \
	else \
		$(CMAKE) -S . -B $(BUILD_DIR) -G $(GENERATOR) $(TOOLCHAIN_FLAG); \
	fi
	@echo "Building with $(NUM_CPUS) parallel jobs..."
	@$(CMAKE) --build $(BUILD_DIR) -v -- -j$(NUM_CPUS)
