from finai_ai.ipc_client import ProposalIPCClient


def test_proposal_contract_has_expected_fields():
    payload = ProposalIPCClient().build_proposal("AAPL", "BUY", 0.82, "Bullish breakout")

    assert payload["version"] == "1.0"
    assert payload["symbol"] == "AAPL"
    assert payload["side"] == "BUY"
    assert payload["confidence"] == 0.82
    assert payload["reason"] == "Bullish breakout"
    assert "timestamp_ms" in payload
