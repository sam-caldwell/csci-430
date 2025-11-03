# File: Makefile.d/docker.mk
#
# Purpose: Build and run a Linux Docker environment to validate the project
#          with the full pipeline under Ubuntu/Clang/LLVM.
#
# Targets:
#  - docker/linux-image: Build the Docker image (ubuntu-based toolchain).
#  - build/linux: Build and run the container, executing the full pipeline.

.PHONY: docker/linux-image build/linux

DOCKER_IMAGE ?= csci-430:linux

docker/linux-image:
	@echo "[docker] Building Linux toolchain image: $(DOCKER_IMAGE)"
	@docker build -t $(DOCKER_IMAGE) .

build/linux: docker/linux-image
	@echo "[docker] Running Linux validation in container..."
	@docker run --rm -t \
	  -e GENERATOR=Ninja \
	  -e TOOLCHAIN= \
	  -v "$(PWD)":/work \
	  -w /work \
	  $(DOCKER_IMAGE)

