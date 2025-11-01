# File: Makefile.d/llvm_prefix.mk
# Purpose: Prefer Homebrew LLVM toolchain if available; export tool paths.

LLVM_PREFIX ?= $(shell brew --prefix llvm@17 2>/dev/null)
ifeq ($(LLVM_PREFIX),)
  ifneq (,$(wildcard /opt/homebrew/opt/llvm@17))
    LLVM_PREFIX := /opt/homebrew/opt/llvm@17
  else ifneq (,$(wildcard /usr/local/opt/llvm@17))
    LLVM_PREFIX := /usr/local/opt/llvm@17
  endif
endif
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

CLANG ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/clang,clang)
CLANGXX ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/clang++,clang++)
LLVM_CONFIG ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/llvm-config,llvm-config)
CLANG_TIDY ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/clang-tidy,clang-tidy)
