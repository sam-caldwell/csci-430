
# Removes the artifact directory and recreates it
clean:
	@echo "Cleaning build/ directory..."
	@rm -rf "$(BUILD_DIR)" build-basic cmake-build-* || true
	@mkdir -p "$(BUILD_DIR)"
	@echo "Recreated build/"
