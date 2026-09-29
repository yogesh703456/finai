#include "trading_core/ipc_contract.h"

#include <cassert>
#include <string>

int main() {
    const std::string payload = R"({
        "version":"1.0",
        "symbol":"AAPL",
        "side":"BUY",
        "confidence":0.82,
        "reason":"Bullish breakout",
        "timestamp_ms":1710000000000
    })";

    trading::core::SignalProposal proposal = trading::core::parse_signal_proposal(payload);

    assert(proposal.symbol == "AAPL");
    assert(proposal.side == "BUY");
    assert(proposal.confidence == 0.82);
    assert(proposal.reason == "Bullish breakout");
    assert(proposal.timestamp_ms > 0);
    assert(trading::core::is_valid_signal_proposal(proposal));

    const trading::core::SignalProposal invalid_proposal{"1.0", "AAPL", "HOLD", 1.5, "Invalid", 1710000000000};
    assert(!trading::core::is_valid_signal_proposal(invalid_proposal));

    return 0;
}
