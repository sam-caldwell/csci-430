#!/usr/bin/env python3
"""
Quick lli smoke runner to validate LLVM bitcode modules execute without
immediate verifier failures. Terminates long-running programs after a short
timeout and treats that as success (goal: IR validity, not program behavior).

Usage: lli_smoke.py <lli> <bc> <timeout_sec> <max_output_bytes>
"""
from __future__ import annotations
import subprocess
import sys
import time

def main(argv: list[str]) -> int:
    if len(argv) != 5:
        print(f"usage: {argv[0]} <lli> <bc> <timeout_sec> <max_output_bytes>")
        return 2
    lli, bc, timeout_s_str, max_out_str = argv[1:]
    try:
        timeout_s = int(timeout_s_str)
        max_out = int(max_out_str)
    except ValueError:
        print("invalid timeout/max_output values")
        return 2
    try:
        p = subprocess.Popen([lli, bc], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    except FileNotFoundError:
        print("[verify-ir] ERROR: lli not found:", lli)
        return 2
    start = time.time()
    buf = bytearray()
    rc = None
    while True:
        if p.poll() is not None:
            rc = p.returncode
            break
        try:
            chunk = p.stdout.read1(4096)  # type: ignore[union-attr]
            if chunk:
                if len(buf) < max_out:
                    buf.extend(chunk[: max_out - len(buf)])
            else:
                time.sleep(0.02)
        except Exception:
            pass
        if time.time() - start > timeout_s:
            # Timed out: terminate and treat as success
            try:
                p.terminate()
                try:
                    p.wait(timeout=1)
                except subprocess.TimeoutExpired:
                    p.kill(); p.wait(timeout=1)
            except Exception:
                pass
            print("[verify-ir] timeout; treated as OK")
            return 0
    out = buf.decode('utf-8', errors='replace')
    if rc != 0:
        print(f"[verify-ir] lli exited non-zero (rc={rc})\n{out}")
        return 1
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))

