from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class MarketSnapshot:
    symbol: str
    price: float
    volume: float
    sentiment: float
    bias: str


class MarketReader:
    """Provides simulated market snapshots for the AI layer."""

    def load_snapshot(self, symbol: str, price: float, volume: float, sentiment: float, bias: str) -> MarketSnapshot:
        return MarketSnapshot(
            symbol=symbol,
            price=float(price),
            volume=float(volume),
            sentiment=float(sentiment),
            bias=bias,
        )
