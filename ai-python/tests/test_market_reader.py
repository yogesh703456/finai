from finai_ai.market_reader import MarketReader


def test_market_reader_creates_snapshot():
    snapshot = MarketReader().load_snapshot("AAPL", 155.5, 2000.0, 0.82, "bullish")

    assert snapshot.symbol == "AAPL"
    assert snapshot.price == 155.5
    assert snapshot.sentiment == 0.82
    assert snapshot.bias == "bullish"
