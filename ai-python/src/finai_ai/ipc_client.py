from __future__ import annotations

import json
from time import time


class ProposalIPCClient:
    """Builds a versioned, schema-defined proposal payload for the C++ core."""

    version = "1.0"

    def build_proposal(self, symbol: str, side: str, confidence: float, reason: str) -> dict:
        payload = {
            "version": self.version,
            "symbol": symbol,
            "side": side,
            "confidence": confidence,
            "reason": reason,
            "timestamp_ms": int(time() * 1000),
        }
        return payload

    def serialize(self, symbol: str, side: str, confidence: float, reason: str) -> str:
        return json.dumps(self.build_proposal(symbol, side, confidence, reason))
