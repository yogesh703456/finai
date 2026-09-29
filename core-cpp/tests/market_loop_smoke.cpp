#include "trading_core/market_loop.h"

#include <cassert>

int main() {
    trading::core::ExecutionEngine engine(100000.0);
    trading::core::MarketLoop loop(engine);

    trading::core::MarketState market{};
    market.symbol = "AAPL";
    market.price = 150.0;

    market.volatility = 0.02;
    market.timestamp_ms = 1710000000000LL;

    trading::core::SignalProposal proposal{};
    proposal.symbol = "AAPL";
    proposal.side = "BUY";
    proposal.confidence = 0.82;

    assert(loop.step(market, proposal));
    assert(loop.portfolio().total_quantity_for("AAPL") == 10);
    assert(loop.portfolio().total_market_value() == 1500.0);
    assert(loop.portfolio().total_exposure() == 1500.0);

    trading::core::MarketState invalid_market{};
    invalid_market.symbol = "AAPL";
    invalid_market.price = 0.0;
    invalid_market.volatility = -0.1;
    assert(!loop.step(invalid_market, proposal));

    return 0;
}
