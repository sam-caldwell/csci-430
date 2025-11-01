# File: Makefile.d/llvm_prefix.mk
#
# Purpose: Prefer Homebrew LLVM toolchain if installed; fall back to system.
#
# Variables:
#  - LLVM_PREFIX: install prefix for llvm binaries (optional)
#
# Attempt to prefer Homebrew LLVM 17 tools for version reporting
LLVM_PREFIX ?= $(shell brew --prefix llvm@17 2>/dev/null)
ifeq ($(LLVM_PREFIX),)
  ifneq (,$(wildcard /opt/homebrew/opt/llvm@17))
    LLVM_PREFIX := /opt/homebrew/opt/llvm@17
  else ifneq (,$(wildcard /usr/local/opt/llvm@17))
    LLVM_PREFIX := /usr/local/opt/llvm@17
  endif
endif
# Fallback to generic llvm if @17 not found
ifeq ($(LLVM_PREFIX),)
  LLVM_PREFIX := $(shell brew --prefix llvm 2>/dev/null)
  ifeq ($(LLVM_PREFIX),)
    ifneq (,$(wildcard /opt/homebrew/opt/llvm))
      LLVM_PREFIX := /opt/homebrew/opt/llvm
    else ifneq (,$(wildcard /usr/local/opt/llvm))
      LLVM_PREFIX := /usr/local/opt/llvm
    endif
  endif
endif
