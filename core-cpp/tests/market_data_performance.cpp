#include "trading_core/market_data_engine.h"

#include <chrono>
#include <iostream>
#include <vector>

#include <windows.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

static size_t current_process_memory_kib() {
    PROCESS_MEMORY_COUNTERS pmc{};
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<size_t>(pmc.WorkingSetSize / 1024ULL);
    }
    return 0ULL;
}

int main() {
    using namespace trading::core;

    MarketDataEngine engine;
    auto provider = std::make_shared<SimulatedStreamingProvider>("SIM");
    engine.set_streaming_provider(provider);
    const auto subscription_id = engine.subscribe("AAPL");

    const std::size_t event_count = 100000;
    std::vector<MarketEvent> events;
    events.reserve(event_count);

    for (std::size_t i = 0; i < event_count; ++i) {
        MarketEvent event;
        event.symbol = "AAPL";
        event.exchange = "SIM";
        event.asset_type = AssetType::Equity;
        event.timestamp_ms = 1710000000000LL + static_cast<long long>(i);
        event.open = 100.0 + static_cast<double>(i % 10);
        event.high = event.open + 1.5;
        event.low = event.open - 1.5;
        event.close = event.open + 0.25;
        event.volume = 1000.0 + static_cast<double>(i);
        events.push_back(event);
    }

    const auto start = std::chrono::high_resolution_clock::now();
    for (const auto& event : events) {
        engine.process_event(event);
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    const double avg_latency_us = static_cast<double>(elapsed_ms) / static_cast<double>(event_count);
    const double throughput_eps = static_cast<double>(event_count) / (static_cast<double>(elapsed_ms) / 1000000.0);
    const size_t memory_kib = current_process_memory_kib();

    std::cout << "subscription_id=" << subscription_id << '\n';
    std::cout << "events_processed=" << event_count << '\n';
    std::cout << "avg_latency_us=" << avg_latency_us << '\n';
    std::cout << "events_per_second=" << throughput_eps << '\n';
    std::cout << "memory_kib=" << memory_kib << '\n';
    std::cout << "latest_close=" << engine.latest_market_state("AAPL").close << '\n';

    return 0;
}
