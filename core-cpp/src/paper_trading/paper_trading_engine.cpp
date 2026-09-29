#include "trading_core/paper_trading_engine.h"

namespace trading::core {

PaperTradingEngine::PaperTradingEngine() : enabled_(true) {}

bool PaperTradingEngine::submit_trade(const std::string& symbol,
                                      int quantity,
                                      double price,
                                      const std::string& side) {
    if (!enabled_ || symbol.empty() || quantity == 0 || price <= 0.0) {
        return false;
    }

    if (side != "BUY" && side != "SELL") {
        return false;
    }

    TradeRecord record{};
    record.symbol = symbol;
    record.quantity = quantity;
    record.price = price;
    record.side = side;
    record.notional = static_cast<double>(quantity) * price;

    trades_.push_back(record);
    return true;
}

std::vector<TradeRecord> PaperTradingEngine::trades() const {
    return trades_;
}

double PaperTradingEngine::total_notional() const {
    double total = 0.0;
    for (const auto& trade : trades_) {
        total += trade.notional;
    }
    return total;
}

TradeSnapshot PaperTradingEngine::snapshot() const {
    TradeSnapshot snapshot{};
    snapshot.trade_count = trades_.size();
    snapshot.total_notional = total_notional();
    if (!trades_.empty()) {
        snapshot.last_symbol = trades_.back().symbol;
    }
    return snapshot;
}

bool PaperTradingEngine::enabled() const noexcept {
    return enabled_;
}

}  // namespace trading::core
