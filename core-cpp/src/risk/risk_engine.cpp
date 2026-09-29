#include "trading_core/risk_engine.h"

#include <algorithm>

namespace trading::core {

RiskEngine::RiskEngine(RiskLimits limits) : limits_(std::move(limits)) {}

bool RiskEngine::should_allow_order(double notional,
                                   double account_value,
                                   double confidence,
                                   double current_drawdown,
                                   double current_position_fraction) const {
    if (account_value <= 0.0) {
        return false;
    }

    const double notional_fraction = notional / account_value;
    const bool within_risk_limit = notional_fraction <= limits_.max_risk_per_trade;
    const bool within_position_limit = current_position_fraction <= limits_.max_position_fraction;
    const bool confidence_ok = confidence >= limits_.min_confidence;
    const bool drawdown_ok = current_drawdown < limits_.max_daily_loss;

    return within_risk_limit && within_position_limit && confidence_ok && drawdown_ok;
}

bool RiskEngine::should_stop_trading(double current_drawdown) const {
    return current_drawdown >= limits_.max_daily_loss;
}

const RiskLimits& RiskEngine::limits() const noexcept {
    return limits_;
}

}  // namespace trading::core
