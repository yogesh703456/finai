import json
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
AI_SRC = os.path.join(ROOT, "ai-python", "src")
if AI_SRC not in sys.path:
    sys.path.insert(0, AI_SRC)

from finai_ai.market_reader import MarketReader
from finai_ai.signal_generator import SignalGenerator
from finai_ai.ipc_client import ProposalIPCClient


def simulate():
    reader = MarketReader()
    generator = SignalGenerator()
    client = ProposalIPCClient()

    ticks = [
        ("AAPL", 154.0, 1800.0, 0.65, "bullish"),
        ("AAPL", 156.8, 2200.0, 0.82, "bullish"),
        ("AAPL", 152.2, 2600.0, 0.34, "bearish"),
        ("MSFT", 420.0, 1200.0, 0.71, "bullish"),
    ]

    results = []
    for symbol, price, volume, sentiment, bias in ticks:
        snapshot = reader.load_snapshot(symbol, price, volume, sentiment, bias)
        signal = generator.generate(snapshot.symbol, snapshot.sentiment, snapshot.bias)
        payload = client.build_proposal(signal.symbol, signal.side, signal.confidence, signal.reason)
        results.append({
            "market": {
                "symbol": snapshot.symbol,
                "price": snapshot.price,
                "volume": snapshot.volume,
                "sentiment": snapshot.sentiment,
                "bias": snapshot.bias,
            },
            "signal": {
                "symbol": signal.symbol,
                "side": signal.side,
                "confidence": signal.confidence,
                "reason": signal.reason,
            },
            "ipc": payload,
            "risk_ready": payload["confidence"] >= 0.6,
        })

    return results


if __name__ == "__main__":
    print(json.dumps(simulate(), indent=2))
