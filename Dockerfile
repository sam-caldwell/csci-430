FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive \
    TZ=UTC \
    CC=clang \
    CXX=clang++

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      ca-certificates \
      build-essential \
      cmake \
      ninja-build \
      shellcheck \
      clang \
      llvm \
      llvm-dev \
      git && \
    bash -lc 'set -eux; \
      LLVMV=$([ -x /usr/bin/clang ] && /usr/bin/clang -dumpversion 2>/dev/null | sed -E "s/\..*//" || echo 18); \
      echo "Detected Clang major version: ${LLVMV}"; \
      found=0; \
      for v in ${LLVMV} 18 17 16 15 14; do \
        echo "Attempting to install LLVM runtimes for v${v}..."; \
        if apt-get update && apt-get install -y --no-install-recommends \
             libclang-rt-${v}-dev \
             libc++-${v}-dev \
             libc++abi-${v}-dev \
             lld-${v} \
             llvm-${v}-tools; then \
          found=1; \
          break; \
        fi; \
      done; \
      if [ "$found" -eq 0 ]; then \
        echo "Falling back to generic libc++/llvm tools"; \
        apt-get update && apt-get install -y --no-install-recommends \
          libc++-dev libc++abi-dev lld llvm; \
      fi' && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /work

# Run the full Linux build pipeline by default when the container starts.
ENTRYPOINT ["/bin/bash", "-lc", "make clean configure lint test demo coverage"]
