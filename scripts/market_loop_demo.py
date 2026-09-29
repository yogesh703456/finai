import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
AI_SRC = os.path.join(ROOT, "ai-python", "src")
if AI_SRC not in sys.path:
    sys.path.insert(0, AI_SRC)

from finai_ai.market_reader import MarketReader
from finai_ai.signal_generator import SignalGenerator
from finai_ai.ipc_client import ProposalIPCClient


def main():
    reader = MarketReader()
    generator = SignalGenerator()
    client = ProposalIPCClient()

    snapshot = reader.load_snapshot("AAPL", 155.5, 2500.0, 0.82, "bullish")
    signal = generator.generate(snapshot.symbol, snapshot.sentiment, snapshot.bias)
    payload = client.build_proposal(signal.symbol, signal.side, signal.confidence, signal.reason)

    print({
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
    })


if __name__ == "__main__":
    main()
