# ARCHITECTURE.md

## System split

```                                                                                      
┌─────────────────────────┐        IPC (versioned,            ┌──────────────────────────-------------------┐
│   Python AI Layer        │        schema-defined)           │   C++ Trading Core                          │
│   (Python 3.11+)          │ ───────────────────────────▶   │   (C++20+)                                  │
│                            │  proposals / signals /         │                                             │
│ - ML training/inference    │  voice commands / queries      │ - Real-time market data                     │
│ - NLP / news intelligence  │ ◀──────────────────────────   │ - Market state                              │
│ - LLM / voice AI            │  market state / fills /       │ - Technical indicators                      │
│ - Research & backtesting   │  risk scores / journal data    │ - Signal execution path                     │
│   experimentation           │                               │ - Validation                                │
│ - Analytics & explanations  │                               │ - Risk engine + position sizing             │
└─────────────────────────┘                                   │ - Adaptive stop-loss                        │
                                                              │ - Paper + live execution                    │
                                                              │ - Order management                          │
                                                              │ - Low-latency monitoring                    │
                                                              │ - Emergency kill switch (Python-independent)│
                                                              └────────────────────────────---------------──┘
                                                                            │
                                                                            ▼
                                                                         Broker
```

## Boundary rules
- Python sends proposals only; C++ decides. Every inbound message is
  re-validated and re-risk-checked inside C++ — Python is never trusted.
- Two IPC lanes:
  - **Hot path** (market-state reads, signal proposals): binary protocol,
    shared memory or Unix domain socket. No HTTP, no serialization overhead
    that risks the latency budget.
  - **Control plane** (config, journal writes, AI explanations, voice
    commands): gRPC/REST is acceptable; not latency-sensitive.
- The emergency kill switch lives entirely in C++ and can halt order flow
  without the IPC layer or Python process being alive.

## Repository shape (proposed, not yet created)
```
core-cpp/        C++20 trading core (CMake project)
  src/
    market_data/
    market_state/
    indicators/
    signal_execution/
    validation/
    risk_engine/
    position_sizing/
    stop_loss/
    paper_trading/
    execution/
    order_management/
    monitoring/
    emergency_stop/
  include/
  tests/

ai-python/       Python 3.11+ AI/research layer
  src/
    ml/
    nlp/
    news_intelligence/
    llm/
    voice_ai/
    research/
    analytics/
    explanations/
  tests/

ipc/             Shared schema/contract definitions (proto/flatbuffers/etc.)
docs/
scripts/
```
