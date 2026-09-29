# DEVELOPMENT.md

## Status
Phase 0 and Phase 1 are complete for the foundational hybrid architecture.
The repository now includes a working C++ core, a Python AI layer, the IPC
boundary contract, and verified smoke tests across both sides. Phase 2 adds
a first working C++ market-data engine with normalized OHLCV events, CSV
historical loading, and a simulated streaming provider for deterministic
development and testing.

## Toolchains (target)
- C++ core: C++20+, CMake ≥3.25, smoke tests via CTest
- Python AI layer: Python 3.11+, pytest, pydantic-settings
- IPC contract: schema-defined proposal payload, versioned and checked into
  `ipc/`

## Working agreement
- No trading, execution, or risk logic is implemented in Python going
  forward — that all moves to C++.
- Any code touching orders, risk, sizing, stops, or the kill switch must
  live in `core-cpp/` once created.
- IPC contracts are defined before either side implements against them.
- Emergency stop design/tests are written and verified in C++ in isolation
  (no Python process required to run them).

## Next step
Phase 2 foundation is now in place: extend the working market-data layer into
richer paper-trading integration and deeper execution/risk logic while
preserving the C++ ownership boundary and the Python-only proposal model.
