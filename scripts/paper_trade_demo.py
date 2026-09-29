import json
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
AI_SRC = os.path.join(ROOT, "ai-python", "src")
if AI_SRC not in sys.path:
    sys.path.insert(0, AI_SRC)

from finai_ai.signal_generator import SignalGenerator
from finai_ai.ipc_client import ProposalIPCClient


def main():
    generator = SignalGenerator()
    client = ProposalIPCClient()

    signal = generator.generate("AAPL", 0.82, "bullish")
    payload = client.build_proposal(signal.symbol, signal.side, signal.confidence, signal.reason)

    print("Signal:")
    print(json.dumps({
        "symbol": signal.symbol,
        "side": signal.side,
        "confidence": signal.confidence,
        "reason": signal.reason,
    }, indent=2))

    print("\nIPC payload:")
    print(json.dumps(payload, indent=2))

    risk_approved = payload["confidence"] >= 0.6 and payload["side"] in {"BUY", "SELL"}
    print(f"\nPaper-trade ready: {risk_approved}")


if __name__ == "__main__":
    main()
