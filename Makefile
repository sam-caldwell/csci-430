# File: Makefile
#
# (c) 2025 Sam Caldwell.  All Rights Reserved.
#
# Purpose: Convenience wrapper for configure/build and environment reporting.
#          Provides 'build', 'clean', and 'version' targets for developers.
include Makefile.d/*.mk

.PHONY: clean build version tree zip lint test help demo unit integration e2e coverage
.DEFAULT_GOAL := help

# Configuration
CMAKE ?= cmake
# Prefer Ninja if available, otherwise fall back to Unix Makefiles
GENERATOR ?= $(shell if command -v ninja >/dev/null 2>&1; then echo Ninja; else echo "Unix Makefiles"; fi)
TOOLCHAIN ?= cmake/Toolchain-HomebrewLLVM.cmake
# Only pass a toolchain if the file exists
TOOLCHAIN_FLAG := $(if $(wildcard $(TOOLCHAIN)),-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN),)
CONFIG ?= Debug
# Derive BUILD_DIR from CONFIG unless overridden by the user
BUILD_ROOT := build
BUILD_SUBDIR := $(shell echo $(CONFIG) | tr A-Z a-z)
BUILD_DIR ?= $(BUILD_ROOT)/cmake-build-$(BUILD_SUBDIR)

CLANG ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/clang,clang)
CLANGXX ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/clang++,clang++)
CLANG_TIDY ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/clang-tidy,clang-tidy)

LLVM_CONFIG ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/llvm-config,llvm-config)
