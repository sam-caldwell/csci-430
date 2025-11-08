# File: Makefile.d/num_cpus.mk
#
# Purpose: Detect CPU cores and enforce safe parallelism.
#
# Policy:
#  - NUM_CPUS passed to any "-j$(NUM_CPUS)" must never exceed half
#    of the host CPU count. This enforcement applies even if a user
#    overrides NUM_CPUS on the command line or via the environment.
#
# Variables (public):
#  - NUM_CPUS: clamped parallel job count (<= floor(host_cpus/2), >= 1)
#
# Variables (internal):
#  - HOST_CPUS: detected logical CPU count
#  - MAX_PARALLEL: floor(HOST_CPUS/2), but at least 1
#  - NUM_CPUS_RAW: optional user override prior to clamping

# Detect host logical CPU count (cross‑platform)
HOST_CPUS := $(shell \
  N=""; \
  if command -v sysctl >/dev/null 2>&1; then N=$$(sysctl -n hw.ncpu 2>/dev/null || true); fi; \
  if [ -z "$$N" ] && command -v nproc >/dev/null 2>&1; then N=$$(nproc 2>/dev/null || true); fi; \
  if [ -z "$$N" ] && command -v getconf >/dev/null 2>&1; then N=$$(getconf _NPROCESSORS_ONLN 2>/dev/null || true); fi; \
  if [ -z "$$N" ]; then N=1; fi; \
  echo $$N)

# Compute maximum allowed parallelism = max(1, floor(HOST_CPUS/2))
MAX_PARALLEL := $(shell H=$(HOST_CPUS); M=$$((H/2)); if [ $$M -lt 1 ]; then echo 1; else echo $$M; fi)

# Capture command-line or environment provided NUM_CPUS (if any)
NUM_CPUS_REQUESTED := $(if $(filter command\ line environment environment\ override,$(origin NUM_CPUS)),$(NUM_CPUS),)

# Allow user override prior to enforcement, then clamp to policy
NUM_CPUS_RAW := $(if $(NUM_CPUS_REQUESTED),$(NUM_CPUS_REQUESTED),$(HOST_CPUS))
# Enforce policy even against command-line overrides
override NUM_CPUS := $(shell J=$(NUM_CPUS_RAW); M=$(MAX_PARALLEL); \
  if [ -z "$$J" ]; then J=$(HOST_CPUS); fi; \
  if [ "$$J" -gt "$$M" ]; then echo $$M; elif [ "$$J" -lt 1 ]; then echo 1; else echo $$J; fi)
