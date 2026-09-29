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


class SimulatedBroker:
    def __init__(self, account_value=100000.0):
        self.account_value = account_value
        self.positions = {}
        self.trades = []

    def process_signal(self, symbol, side, confidence, price):
        if confidence < 0.6:
            return {"accepted": False, "reason": "confidence too low"}

        qty = 10
        notional = qty * price
        if notional / self.account_value > 0.02:
            return {"accepted": False, "reason": "risk cap exceeded"}

        self.positions[symbol] = self.positions.get(symbol, 0) + (qty if side == "BUY" else -qty)
        self.trades.append({
            "symbol": symbol,
            "side": side,
            "quantity": qty,
            "price": price,
            "notional": notional,
        })
        return {"accepted": True, "quantity": qty, "notional": notional}


def run_loop():
    reader = MarketReader()
    generator = SignalGenerator()
    client = ProposalIPCClient()
    broker = SimulatedBroker()

    ticks = [
        ("AAPL", 154.0, 1800.0, 0.65, "bullish"),
        ("AAPL", 156.8, 2200.0, 0.82, "bullish"),
        ("AAPL", 152.2, 2600.0, 0.34, "bearish"),
        ("MSFT", 420.0, 1200.0, 0.71, "bullish"),
    ]

    results = []
    for symbol, price, volume, sentiment, bias in ticks:
        snapshot = reader.load_snapshot(symbol, price, volume, sentiment, bias)
        signal = generator.generate(symbol, snapshot.sentiment, snapshot.bias)
        payload = client.build_proposal(signal.symbol, signal.side, signal.confidence, signal.reason)
        decision = broker.process_signal(payload["symbol"], payload["side"], payload["confidence"], price)
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
            "decision": decision,
            "positions": broker.positions.copy(),
            "trades": broker.trades.copy(),
        })

    return results


if __name__ == "__main__":
    print(json.dumps(run_loop(), indent=2))
