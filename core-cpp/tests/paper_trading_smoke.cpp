#include "trading_core/paper_trading_engine.h"

#include <cassert>

int main() {
    trading::core::PaperTradingEngine engine;

    assert(engine.enabled());
    assert(engine.submit_trade("AAPL", 10, 150.0, "BUY"));
    assert(engine.submit_trade("MSFT", 5, 300.0, "SELL"));
    assert(!engine.submit_trade("", 2, 10.0, "BUY"));
    assert(!engine.submit_trade("NVDA", 0, 100.0, "BUY"));
    assert(!engine.submit_trade("NVDA", 3, 0.0, "BUY"));
    assert(!engine.submit_trade("NVDA", 3, 100.0, "HOLD"));

    const auto trades = engine.trades();
    assert(trades.size() == 2);
    assert(engine.total_notional() == 1500.0 + 1500.0);

    const auto snapshot = engine.snapshot();
    assert(snapshot.trade_count == 2);
    assert(snapshot.total_notional == 3000.0);
    assert(snapshot.last_symbol == "MSFT");

    return 0;
}
