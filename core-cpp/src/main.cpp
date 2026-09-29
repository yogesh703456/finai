#include "trading_core/risk_engine.h"

#include <iostream>

int main() {
    trading::core::RiskEngine risk_engine{};

    const bool approved = risk_engine.should_allow_order(
        5000.0,
        100000.0,
        0.72,
        0.02,
        0.04);

    std::cout << "Order approved: " << (approved ? "yes" : "no") << '\n';
    std::cout << "Daily stop triggered: " << (risk_engine.should_stop_trading(0.03) ? "yes" : "no")
              << '\n';

    return 0;
}
