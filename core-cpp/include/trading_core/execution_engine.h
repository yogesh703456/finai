#pragma once

#include "trading_core/ipc_contract.h"
#include "trading_core/paper_trading_engine.h"
#include "trading_core/risk_engine.h"

#include <string>

namespace trading::core {

struct ExecutionDecision {
    bool accepted = false;
    bool paper_executed = false;
    int quantity = 0;
    double notional = 0.0;
    std::string reason;
};

class ExecutionEngine {
public:
    ExecutionEngine(double account_value,
                    RiskEngine risk_engine = RiskEngine{},
                    PaperTradingEngine paper_engine = PaperTradingEngine{});

    ExecutionDecision evaluate(const SignalProposal& proposal,
                               double current_price,
                               double current_drawdown,
                               double current_position_fraction) const;

    const PaperTradingEngine& paper_trading() const noexcept;

private:
    double account_value_;
    RiskEngine risk_engine_;
    mutable PaperTradingEngine paper_engine_;
};

}  // namespace trading::core
