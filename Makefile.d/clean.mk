# Removes the artifact directory and recreates it
clean:
	@echo "Cleaning build/ directory..."
	@rm -rf build build-basic cmake-build-* || true
	@mkdir -p build
	@echo "Recreated build/"
