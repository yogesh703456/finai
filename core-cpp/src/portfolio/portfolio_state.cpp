#include "trading_core/portfolio_state.h"

namespace trading::core {

void PortfolioState::add_position(const std::string& symbol, int quantity, double entry_price, double market_price) {
    Position position{};
    position.symbol = symbol;
    position.quantity = quantity;
    position.entry_price = entry_price;
    position.market_value = market_price * static_cast<double>(quantity);
    positions_[symbol] = position;
}

void PortfolioState::update_market_value(const std::string& symbol, double market_price) {
    auto it = positions_.find(symbol);
    if (it == positions_.end()) {
        return;
    }
    it->second.market_value = market_price * static_cast<double>(it->second.quantity);
}

double PortfolioState::total_market_value() const {
    double total = 0.0;
    for (const auto& [_, position] : positions_) {
        total += position.market_value;
    }
    return total;
}

double PortfolioState::total_exposure() const {
    double total = 0.0;
    for (const auto& [_, position] : positions_) {
        total += std::abs(position.market_value);
    }
    return total;
}

int PortfolioState::total_quantity_for(const std::string& symbol) const {
    const auto it = positions_.find(symbol);
    if (it == positions_.end()) {
        return 0;
    }
    return it->second.quantity;
}

const std::map<std::string, Position>& PortfolioState::positions() const noexcept {
    return positions_;
}

}  // namespace trading::core
