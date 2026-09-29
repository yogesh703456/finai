#include "trading_core/market_loop.h"

namespace trading::core {

MarketLoop::MarketLoop(ExecutionEngine execution_engine, PortfolioState portfolio_state)
    : execution_engine_(std::move(execution_engine)),
      portfolio_state_(std::move(portfolio_state)) {}

bool MarketLoop::step(const MarketState& market_state, const SignalProposal& proposal) {
    if (market_state.symbol.empty() || market_state.price <= 0.0 || market_state.volatility < 0.0 ||
        market_state.timestamp_ms <= 0) {
        return false;
    }

    const auto decision = execution_engine_.evaluate(proposal, market_state.price, 0.02, 0.04);
    if (!decision.accepted || !decision.paper_executed) {
        return false;
    }

    int qty = decision.quantity;
    if (proposal.side == "SELL") {
        qty = -qty;
    }

    portfolio_state_.add_position(proposal.symbol, qty, market_state.price, market_state.price);
    portfolio_state_.update_market_value(proposal.symbol, market_state.price);
    return true;
}

const PortfolioState& MarketLoop::portfolio() const noexcept {
    return portfolio_state_;
}

}  // namespace trading::core
