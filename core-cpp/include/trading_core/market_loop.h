#pragma once

#include "trading_core/execution_engine.h"
#include "trading_core/market_state.h"
#include "trading_core/portfolio_state.h"

namespace trading::core {

class MarketLoop {
public:
    MarketLoop(ExecutionEngine execution_engine, PortfolioState portfolio_state = PortfolioState{});

    bool step(const MarketState& market_state, const SignalProposal& proposal);
    const PortfolioState& portfolio() const noexcept;

private:
    ExecutionEngine execution_engine_;
    PortfolioState portfolio_state_;
};

}  // namespace trading::core
