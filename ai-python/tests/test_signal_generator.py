from finai_ai.signal_generator import SignalGenerator


def test_bullish_signal_generation():
    proposal = SignalGenerator().generate("AAPL", 0.8, "bullish")
    assert proposal.symbol == "AAPL"
    assert proposal.side == "BUY"
    assert proposal.confidence >= 0.7


def test_bearish_signal_generation():
    proposal = SignalGenerator().generate("TSLA", 0.2, "bearish")
    assert proposal.symbol == "TSLA"
    assert proposal.side == "SELL"
    assert proposal.confidence >= 0.7


def test_neutral_signal_generation():
    proposal = SignalGenerator().generate("MSFT", 0.5, "neutral")
    assert proposal.side == "SELL"
    assert proposal.confidence == 0.5
