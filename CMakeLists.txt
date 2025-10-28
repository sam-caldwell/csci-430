#! File: CMakeLists.txt
#! (c) 2025 Sam Caldwell.  All Rights Reserved.
#! Purpose: Top-level CMake entry for the csci-430 repository.
#!          Orchestrates modular configuration and project targets.

cmake_minimum_required(VERSION 3.21)

# Detect and prefer Homebrew LLVM toolchain and utilities
include(cmake/LLVMConfig.cmake)

project(csci_430 LANGUAGES C CXX)

# Project-wide configuration
include(cmake/BuildConfig.cmake)

# Testing configuration (GoogleTest + CTest)
include(cmake/Testing.cmake)

# Helper macros and project wrapper
include(cmake/ProjectHelpers.cmake)

# Define targets
include(cmake/projects.cmake)
