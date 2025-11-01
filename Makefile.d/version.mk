# Show toolchain and system versions
version:
	@echo "==== CMake ===="
	@cmake --version | sed -n '1,3p'
	@echo
	@echo "==== Clang / LLVM ===="
	@echo "clang:   $$($(CLANG) --version 2>/dev/null | head -n1 || echo 'not found')"
	@echo "clang++: $$($(CLANGXX) --version 2>/dev/null | head -n1 || echo 'not found')"
	@printf "llvm-config: "; $(LLVM_CONFIG) --version 2>/dev/null || echo 'not found'
	@brew list --versions llvm@17 2>/dev/null || true
	@brew list --versions llvm 2>/dev/null || true
	@echo
	@echo "==== Ninja ===="
	@printf "ninja "; ninja --version 2>/dev/null || echo 'not found'
	@echo
	@echo "==== Operating System ===="
	@sw_vers 2>/dev/null || true
	@uname -mrsv
	@echo
	@echo "==== CPU ===="
	@printf "arch: "; uname -m
	@printf "model: "; sysctl -n hw.model 2>/dev/null || echo '(unavailable)'
	@printf "brand: "; sysctl -n machdep.cpu.brand_string 2>/dev/null || echo '(unavailable)'
	@printf "cores: "; sysctl -n hw.ncpu 2>/dev/null || echo '(unavailable)'
	@echo
	@echo "==== Xcode ===="
	@xcodebuild -version 2>/dev/null || echo 'Xcode not installed'
