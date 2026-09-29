# PROJECT_SPEC.md
## AI-Powered Market Intelligence & Algorithmic Trading Platform

Status: **Phase 0 — Spec revised to hybrid architecture. No implementation yet.**

## 1. Vision
Same target capabilities as before (historical data, real-time monitoring,
news intelligence, pattern matching, signals, risk scoring, adaptive stops,
backtesting, paper/live trading, monitoring, voice, journal, AI explanations)
— now split across a latency-critical C++ core and a Python AI/research layer.

## 2. Hybrid Architecture Decision

The platform is split into two independently-deployable systems:

**C++ Trading Core (C++20+)** — owns everything on the latency-critical /
money-moving path:
real-time market data, market state, technical indicators, signal execution
path, validation, risk engine, position sizing, adaptive stop-loss, paper
trading execution, live broker execution, order management, low-latency
monitoring, and the **emergency kill switch**.

**Python AI Layer (Python 3.11+)** — owns everything analytical/generative:
machine learning, model training, NLP, news intelligence, LLM, voice AI,
research, experimentation, analytics, AI explanations.

```
Python AI
    ↓
Controlled IPC/API
    ↓
C++ Trading Core
    ↓
Risk Engine
    ↓
Execution
    ↓
Broker
```

Python never calls a broker, never bypasses risk checks, and has no code
path capable of placing or modifying an order directly. It only sends
*proposals* (signals, model scores, voice commands) across the IPC boundary;
the C++ core validates, risk-checks, sizes, and decides.

## 3. IPC Boundary
- One-directional trust: C++ never trusts Python input as authoritative —
  every message is re-validated and re-risk-checked in C++.
- Low-latency channel for the hot path (proposals, market-state queries):
  binary protocol over shared memory or a local socket (e.g. Unix domain
  socket / shared-memory ring buffer), not HTTP, on this path.
- Higher-latency channel for control/config/journal/explanations: a
  local API (e.g. gRPC or REST) is acceptable.
- All IPC messages are versioned, schema-defined, and logged.

## 4. Emergency Kill Switch
Implemented **entirely in C++**, with zero dependency on Python, the IPC
layer being up, or any LLM. It can halt order flow even if Python is dead,
frozen, or compromised. Trigger sources: internal C++ risk breach, a local
hardware/OS signal, or an IPC "stop" message — but the switch's own logic
never *requires* the IPC layer to function.

## 5. Hard Rules (updated)
Rules 1–3, 8–14 unchanged. Rules 4–7 restated for the hybrid model:
4. Live trading OFF by default (C++ core config flag).
5. Paper trading required before live trading (enforced in C++ core).
6. LLM/Python can never bypass risk controls or call the broker directly —
   only the C++ core executes orders.
7. Emergency stop is implemented in C++, independent of Python/LLM.

## 6. Tech Stack
- Core: C++20+, CMake, a low-latency IPC library (to be selected in Phase 1)
- AI layer: Python 3.11+, pydantic-settings, pytest
- IPC: binary/shared-memory for hot path; gRPC/REST for control plane

## 7. Roadmap (revised)
Phase 1 will define the C++ core skeleton, Python AI skeleton, and the IPC
contract — no trading logic yet. Later phases build out each side per the
responsibility split in Section 2, always keeping the emergency stop and
order execution exclusively in C++.
