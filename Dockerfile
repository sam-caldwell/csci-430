FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive \
    TZ=UTC \
    CC=clang \
    CXX=clang++

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      build-essential \
      cmake \
      ninja-build \
      clang \
      llvm \
      llvm-dev \
      git \
      ca-certificates && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /work

# Run the full Linux build pipeline by default when the container starts.
ENTRYPOINT ["/bin/bash", "-lc", "make clean configure lint test demo coverage"]

