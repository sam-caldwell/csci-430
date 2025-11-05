FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive \
    TZ=UTC \
    CC=clang-17 \
    CXX=clang++-17 \
    PATH=/usr/lib/llvm-17/bin:$PATH

RUN set -eux; \
    apt-get update; \
    apt-get install -y --no-install-recommends \
      wget \
      software-properties-common \
      lsb-release \
      gnupg \
      ca-certificates \
      build-essential \
      cmake \
      ninja-build \
      shellcheck \
      git; \
    # Install pinned LLVM/Clang 17 toolchain from apt.llvm.org
    wget https://apt.llvm.org/llvm.sh; \
    chmod +x llvm.sh; \
    ./llvm.sh 17; \
    apt-get install -y --no-install-recommends \
      clang-17 \
      clang-tidy-17 \
      llvm-17-tools \
      libc++-17-dev \
      libc++abi-17-dev \
      lld-17; \
    rm -rf /var/lib/apt/lists/*

WORKDIR /work

# Run the full Linux build pipeline by default when the container starts.
ENTRYPOINT ["/bin/bash", "-lc", "make clean configure lint test demo coverage"]
