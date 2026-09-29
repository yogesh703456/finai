#include "trading_core/execution_engine.h"

#include <cassert>

int main() {
    trading::core::ExecutionEngine engine(100000.0);
    trading::core::SignalProposal proposal{};
    proposal.symbol = "AAPL";
    proposal.side = "BUY";
    proposal.confidence = 0.82;

    const auto decision = engine.evaluate(proposal, 150.0, 0.02, 0.04);

    assert(decision.accepted);
    assert(decision.paper_executed);
    assert(decision.quantity == 10);
    assert(engine.paper_trading().trades().size() == 1);

    return 0;
}
