#include "trading_core/market_data_engine.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

int main() {
    using namespace trading::core;

    {  // valid event
        MarketEvent event;
        event.symbol = "AAPL";
        event.exchange = "NASDAQ";
        event.asset_type = AssetType::Equity;
        event.timestamp_ms = 1710000000000LL;
        event.open = 100.0;
        event.high = 105.0;
        event.low = 99.0;
        event.close = 103.5;
        event.volume = 2500.0;
        assert(MarketDataValidator::is_valid(event));
    }

    {  // invalid event
        MarketEvent invalid{};
        invalid.symbol = "AAPL";
        invalid.exchange = "NASDAQ";
        invalid.asset_type = AssetType::Equity;
        invalid.timestamp_ms = 1710000000000LL;
        invalid.open = -1.0;
        invalid.high = 0.0;
        invalid.low = 0.0;
        invalid.close = 0.0;
        invalid.volume = -10.0;
        assert(!MarketDataValidator::is_valid(invalid));
    }

    {  // csv load
        const fs::path csv = fs::temp_directory_path() / "finai_market_data_test.csv";
        std::ofstream out(csv);
        out << "symbol,exchange,asset_type,timestamp_ms,open,high,low,close,volume\n";
        out << "AAPL,NASDAQ,EQUITY,1710000000000,100,105,99,103.5,2500\n";
        out << "AAPL,NASDAQ,EQUITY,1710000000100,103.5,106,101,104.0,2200\n";
        out << "MSFT,NASDAQ,EQUITY,1710000000200,250,255,248,252.0,1800\n";
        out.close();

        CsvHistoricalProvider provider(csv.string());
        const auto history = provider.load_history("AAPL", 1710000000000LL, 1710000000200LL);
        assert(history.size() == 2);
        assert(provider.connection_status().healthy);
        fs::remove(csv);
    }

    {  // malformed csv row should fail gracefully
        const fs::path csv = fs::temp_directory_path() / "finai_market_data_bad.csv";
        std::ofstream out(csv);
        out << "symbol,exchange,asset_type,timestamp_ms,open,high,low,close,volume\n";
        out << "AAPL,NASDAQ,EQUITY,1710000000000,100,105,99,103.5,2500\n";
        out << "BAD,NOPE,UNKNOWN,abc,1,2,3,4,5\n";
        out.close();

        CsvHistoricalProvider provider(csv.string());
        const auto history = provider.load_history("AAPL", 1710000000000LL, 1710000001200LL);
        assert(history.size() == 1);
        fs::remove(csv);
    }

    {  // multiple symbols and ordering
        MarketDataEngine engine;
        MarketEvent a {"AAPL", "NASDAQ", AssetType::Equity, 1710000000000LL, 100.0, 101.0, 99.0, 100.5, 1000.0};
        MarketEvent b {"MSFT", "NASDAQ", AssetType::Equity, 1710000000100LL, 250.0, 252.0, 249.0, 251.0, 1500.0};
        assert(engine.process_event(a));
        assert(engine.process_event(b));
        assert(engine.latest_market_state("AAPL").symbol == "AAPL");
        assert(engine.latest_market_state("MSFT").symbol == "MSFT");
    }

    {  // streaming delivery
        auto provider = std::make_shared<SimulatedStreamingProvider>("SIM");
        MarketDataEngine engine;
        engine.set_historical_provider(std::make_shared<CsvHistoricalProvider>(""));
        engine.set_streaming_provider(provider);
        const auto sub = engine.subscribe("AAPL");
        provider->publish(MarketEvent{"AAPL", "SIM", AssetType::Equity, 1710000000000LL, 10.0, 11.0, 9.5, 10.4, 500.0});
        provider->publish(MarketEvent{"AAPL", "SIM", AssetType::Equity, 1710000000100LL, 10.4, 10.8, 10.2, 10.6, 600.0});
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        assert(engine.latest_market_state("AAPL").close == 10.6);
        assert(sub == 0);
    }

    {  // provider failure and recovery
        FailingStreamingProvider provider;
        assert(!provider.connection_status().healthy);
        provider.set_healthy(true);
        assert(provider.connection_status().healthy);
    }

    {  // thread safety iteration
        SimulatedStreamingProvider provider("TS");
        MarketDataEngine engine;
        engine.set_streaming_provider(std::make_shared<SimulatedStreamingProvider>("TS"));

        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&engine, i]() {
                for (int j = 0; j < 50; ++j) {
                    MarketEvent ev{"AAPL", "SIM", AssetType::Equity, 1710000000000LL + j + i, 100.0 + j, 101.0 + j, 99.0 + j, 100.5 + j, 1000.0 + j};
                    engine.process_event(ev);
                }
            });
        }
        for (auto& thread : threads) {
            thread.join();
        }
        assert(engine.latest_market_state("AAPL").timestamp_ms >= 1710000000000LL);
    }

    return 0;
}
