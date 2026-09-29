# IPC contract

This folder holds versioned message schemas for the trust boundary between the Python AI layer and the C++ trading core.

## Planned zones

- hot_path: low-latency binary proposals and market-state requests
- control_plane: config, journal, explanations, and orchestration messages

## Phase 0/1 schema

The project now includes a versioned proposal schema at
`ipc/signal_proposal.proto` that defines the trust-boundary payload expected by
both the Python AI layer and the C++ core.

```proto
message SignalProposal {
  string version = 1;
  string symbol = 2;
  string side = 3; // BUY | SELL
  double confidence = 4;
  string reason = 5;
  uint64 timestamp_ms = 6;
}
```

This is the current contract baseline for the hybrid architecture. The next phase
will expand the schema and operational semantics for richer trading and control
messages without changing the core trust rule: Python proposes; C++ validates,
risks, and executes.
