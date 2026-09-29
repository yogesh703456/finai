#pragma once

namespace trading::core {

struct RiskLimits {
    double max_risk_per_trade = 0.02;
    double max_position_fraction = 0.10;
    double min_confidence = 0.60;
    double max_daily_loss = 0.05;
};

class RiskEngine {
public:
    explicit RiskEngine(RiskLimits limits = RiskLimits{});

    bool should_allow_order(double notional,
                            double account_value,
                            double confidence,
                            double current_drawdown,
                            double current_position_fraction) const;

    bool should_stop_trading(double current_drawdown) const;

    const RiskLimits& limits() const noexcept;

private:
    RiskLimits limits_;
};

}  // namespace trading::core
