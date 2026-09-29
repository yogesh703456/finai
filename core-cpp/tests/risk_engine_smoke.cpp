#include "trading_core/risk_engine.h"

#include <cassert>

int main() {
    trading::core::RiskEngine engine{};

    assert(!engine.should_allow_order(5000.0, 100000.0, 0.75, 0.02, 0.04));
    assert(engine.should_allow_order(1500.0, 100000.0, 0.75, 0.02, 0.04));
    assert(!engine.should_allow_order(30000.0, 100000.0, 0.75, 0.02, 0.04));
    assert(!engine.should_allow_order(5000.0, 100000.0, 0.30, 0.02, 0.04));
    assert(engine.should_stop_trading(0.05));
    assert(!engine.should_stop_trading(0.04));

    return 0;
}
