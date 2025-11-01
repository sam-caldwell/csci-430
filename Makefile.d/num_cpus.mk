# Parallel build settings: detect CPU cores cross-platform
# Allows override: make build NUM_CPUS=8
NUM_CPUS ?= $(shell \
  N=""; \
  if command -v sysctl >/dev/null 2>&1; then N=$$(sysctl -n hw.ncpu 2>/dev/null || true); fi; \
  if [ -z "$$N" ] && command -v nproc >/dev/null 2>&1; then N=$$(nproc 2>/dev/null || true); fi; \
  if [ -z "$$N" ] && command -v getconf >/dev/null 2>&1; then N=$$(getconf _NPROCESSORS_ONLN 2>/dev/null || true); fi; \
  if [ -z "$$N" ]; then N=1; fi; \
  echo $$N)

