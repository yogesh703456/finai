from dataclasses import dataclass
from typing import Literal


@dataclass(frozen=True)
class SignalProposal:
    symbol: str
    side: Literal["BUY", "SELL"]
    confidence: float
    reason: str


class SignalGenerator:
    """Generates proposals for the C++ execution engine."""

    def generate(self, symbol: str, sentiment_score: float, market_bias: str) -> SignalProposal:
        if sentiment_score >= 0.7 and market_bias.upper() == "BULLISH":
            return SignalProposal(
                symbol=symbol,
                side="BUY",
                confidence=min(sentiment_score, 1.0),
                reason="Bullish momentum and positive sentiment.",
            )

        if sentiment_score <= 0.3 and market_bias.upper() == "BEARISH":
            return SignalProposal(
                symbol=symbol,
                side="SELL",
                confidence=min(1.0 - sentiment_score, 1.0),
                reason="Weak sentiment and bearish pressure.",
            )

        return SignalProposal(
            symbol=symbol,
            side="BUY" if market_bias.upper() == "BULLISH" else "SELL",
            confidence=0.5,
            reason="Neutral market posture; proposal remains conservative.",
        )
