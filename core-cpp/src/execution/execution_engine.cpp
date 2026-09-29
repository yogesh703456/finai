#include "trading_core/execution_engine.h"

#include <string>

namespace trading::core {

ExecutionEngine::ExecutionEngine(double account_value,
                                 RiskEngine risk_engine,
                                 PaperTradingEngine paper_engine)
    : account_value_(account_value),
      risk_engine_(std::move(risk_engine)),
      paper_engine_(std::move(paper_engine)) {}

ExecutionDecision ExecutionEngine::evaluate(const SignalProposal& proposal,
                                           double current_price,
                                           double current_drawdown,
                                           double current_position_fraction) const {
    ExecutionDecision decision{};

    if (proposal.symbol.empty() || proposal.side.empty()) {
        decision.reason = "Invalid proposal";
        return decision;
    }

    const double notional = current_price * 10.0;
    const bool allowed = risk_engine_.should_allow_order(
        notional,
        account_value_,
        proposal.confidence,
        current_drawdown,
        current_position_fraction);

    if (!allowed) {
        decision.reason = "Rejected by risk engine";
        return decision;
    }

    decision.accepted = true;
    decision.quantity = 10;
    decision.notional = notional;
    decision.reason = "Risk accepted; simulated paper order created";

    const bool sent = paper_engine_.submit_trade(proposal.symbol, decision.quantity, current_price, proposal.side);
    if (sent) {
        decision.paper_executed = true;
    }
    return decision;
}

const PaperTradingEngine& ExecutionEngine::paper_trading() const noexcept {
    return paper_engine_;
}

}  // namespace trading::core
