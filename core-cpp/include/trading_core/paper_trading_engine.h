#pragma once

#include <string>
#include <vector>

namespace trading::core {

struct TradeRecord {
    std::string symbol;
    int quantity = 0;
    double price = 0.0;
    double notional = 0.0;
    std::string side;
};

struct TradeSnapshot {
    std::size_t trade_count = 0;
    double total_notional = 0.0;
    std::string last_symbol;
};

class PaperTradingEngine {
public:
    PaperTradingEngine();

    bool submit_trade(const std::string& symbol, int quantity, double price, const std::string& side);
    std::vector<TradeRecord> trades() const;
    double total_notional() const;
    TradeSnapshot snapshot() const;
    bool enabled() const noexcept;

private:
    std::vector<TradeRecord> trades_;
    bool enabled_;
};

}  // namespace trading::core
