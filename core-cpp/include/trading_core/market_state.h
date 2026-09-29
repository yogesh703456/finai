#pragma once

#include <string>

namespace trading::core {

struct MarketState {
    std::string symbol;
    double price = 0.0;
    double bid = 0.0;
    double ask = 0.0;
    double volume = 0.0;
    double volatility = 0.0;
    long long timestamp_ms = 0;
};

}  // namespace trading::core
