#pragma once

#include <string>
#include <map>

namespace trading::core {

struct Position {
    std::string symbol;
    int quantity = 0;
    double entry_price = 0.0;
    double market_value = 0.0;
};

class PortfolioState {
public:
    void add_position(const std::string& symbol, int quantity, double entry_price, double market_price);
    void update_market_value(const std::string& symbol, double market_price);
    double total_market_value() const;
    double total_exposure() const;
    int total_quantity_for(const std::string& symbol) const;
    const std::map<std::string, Position>& positions() const noexcept;

private:
    std::map<std::string, Position> positions_;
};

}  // namespace trading::core
