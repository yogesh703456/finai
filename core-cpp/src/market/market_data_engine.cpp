#include "trading_core/market_data_engine.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace trading::core {
namespace {

std::string to_upper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return value;
}

std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> values;
    std::stringstream stream(line);
    std::string item;
    while (std::getline(stream, item, ',')) {
        values.push_back(item);
    }
    return values;
}

}  // namespace

bool MarketDataValidator::is_valid(const MarketEvent& event) {
    if (event.symbol.empty() || event.exchange.empty()) {
        return false;
    }
    if (event.timestamp_ms <= 0) {
        return false;
    }
    if (event.volume < 0.0) {
        return false;
    }
    if (event.open < 0.0 || event.high < 0.0 || event.low < 0.0 || event.close < 0.0) {
        return false;
    }
    if (event.low > event.high) {
        return false;
    }
    if (event.open < event.low || event.open > event.high) {
        return false;
    }
    if (event.close < event.low || event.close > event.high) {
        return false;
    }
    if (event.high < std::max(event.open, event.close) || event.low > std::min(event.open, event.close)) {
        return false;
    }
    return true;
}

AssetType MarketDataValidator::parse_asset_type(const std::string& value) {
    const std::string normalized = to_upper(value);
    if (normalized == "EQUITY" || normalized == "STOCK") {
        return AssetType::Equity;
    }
    if (normalized == "FOREX" || normalized == "FX") {
        return AssetType::Forex;
    }
    if (normalized == "CRYPTO" || normalized == "CRYPTOCURRENCY") {
        return AssetType::Crypto;
    }
    if (normalized == "COMMODITY" || normalized == "FUTURE" || normalized == "FUTURES") {
        return AssetType::Commodity;
    }
    if (normalized == "INDEX" || normalized == "ETF") {
        return AssetType::Index;
    }
    return AssetType::Unknown;
}

std::string MarketDataValidator::asset_type_to_string(AssetType value) {
    switch (value) {
        case AssetType::Equity:
            return "EQUITY";
        case AssetType::Forex:
            return "FOREX";
        case AssetType::Crypto:
            return "CRYPTO";
        case AssetType::Commodity:
            return "COMMODITY";
        case AssetType::Index:
            return "INDEX";
        case AssetType::Unknown:
        default:
            return "UNKNOWN";
    }
}

CsvHistoricalProvider::CsvHistoricalProvider(std::string csv_path) : csv_path_(std::move(csv_path)) {
    connect();
}

bool CsvHistoricalProvider::connect() {
    connected_ = !csv_path_.empty();
    if (!connected_) {
        status_ = {false, "CSV_PATH_NOT_SET", "No CSV path configured"};
        return false;
    }
    std::ifstream input(csv_path_);
    if (!input.is_open()) {
        connected_ = false;
        status_ = {false, "CSV_OPEN_FAILED", "Unable to open historical CSV file"};
        return false;
    }
    status_ = {true, "CSV_READY", ""};
    return true;
}

void CsvHistoricalProvider::disconnect() {
    connected_ = false;
    status_ = {false, "DISCONNECTED", "Historical CSV provider disconnected"};
}

ProviderStatus CsvHistoricalProvider::connection_status() const {
    return status_;
}

std::vector<MarketEvent> CsvHistoricalProvider::load_history(const std::string& symbol,
                                                           long long start_ms,
                                                           long long end_ms) {
    if (!connected_ || csv_path_.empty()) {
        return {};
    }

    std::ifstream input(csv_path_);
    if (!input.is_open()) {
        status_ = {false, "CSV_OPEN_FAILED", "Unable to open historical CSV file"};
        return {};
    }

    std::vector<MarketEvent> results;
    std::string line;
    bool header_seen = false;

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        const auto values = split_csv_line(line);
        if (!header_seen) {
            header_seen = true;
            if (values.size() >= 9 && to_upper(values[0]) == "SYMBOL") {
                continue;
            }
        }

        if (values.size() != 9) {
            continue;
        }

        try {
            MarketEvent event;
            event.symbol = values[0];
            event.exchange = values[1];
            event.asset_type = MarketDataValidator::parse_asset_type(values[2]);
            event.timestamp_ms = std::stoll(values[3]);
            event.open = std::stod(values[4]);
            event.high = std::stod(values[5]);
            event.low = std::stod(values[6]);
            event.close = std::stod(values[7]);
            event.volume = std::stod(values[8]);

            if (symbol.empty() || event.symbol == symbol) {
                if (event.timestamp_ms >= start_ms && event.timestamp_ms <= end_ms && MarketDataValidator::is_valid(event)) {
                    results.push_back(event);
                }
            }
        } catch (const std::exception&) {
            continue;
        }
    }

    std::sort(results.begin(), results.end(), [](const MarketEvent& lhs, const MarketEvent& rhs) {
        return lhs.timestamp_ms < rhs.timestamp_ms;
    });
    status_ = {true, "CSV_LOADED", ""};
    return results;
}

SimulatedStreamingProvider::SimulatedStreamingProvider(std::string exchange_name)
    : exchange_name_(std::move(exchange_name)) {
    connect();
}

bool SimulatedStreamingProvider::connect() {
    connected_ = true;
    healthy_ = true;
    status_ = {true, "SIMULATED_STREAMING_PROVIDER_READY", ""};
    return true;
}

void SimulatedStreamingProvider::disconnect() {
    connected_ = false;
    healthy_ = false;
    status_ = {false, "DISCONNECTED", "Simulated streaming provider disconnected"};
}

ProviderStatus SimulatedStreamingProvider::connection_status() const {
    return {healthy_ && connected_, healthy_ && connected_ ? "SIMULATED_STREAMING_PROVIDER_READY" : "SIMULATED_STREAMING_PROVIDER_UNHEALTHY",
            healthy_ && connected_ ? "" : "Provider not healthy"};
}

bool SimulatedStreamingProvider::subscribe(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = std::find(subscriptions_.begin(), subscriptions_.end(), symbol);
    if (it == subscriptions_.end()) {
        subscriptions_.push_back(symbol);
    }
    return true;
}

bool SimulatedStreamingProvider::unsubscribe(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = std::remove(subscriptions_.begin(), subscriptions_.end(), symbol);
    subscriptions_.erase(it, subscriptions_.end());
    return true;
}

void SimulatedStreamingProvider::publish(const MarketEvent& event) {
    if (!connected_ || !healthy_ || !callback_) {
        return;
    }
    callback_(event);
}

void SimulatedStreamingProvider::set_healthy(bool healthy) {
    healthy_ = healthy;
    status_ = {healthy_ && connected_, healthy_ && connected_ ? "SIMULATED_STREAMING_PROVIDER_READY" : "SIMULATED_STREAMING_PROVIDER_UNHEALTHY",
               healthy_ && connected_ ? "" : "Simulated provider health changed"};
}

FailingStreamingProvider::FailingStreamingProvider() : SimulatedStreamingProvider("FAIL") {
    set_healthy(false);
}

ProviderStatus FailingStreamingProvider::connection_status() const {
    return {healthy_ && connected_, healthy_ && connected_ ? "FAILING_PROVIDER_RECOVERED" : "FAILURE_MODE",
            healthy_ && connected_ ? "" : "Provider intentionally unhealthy"};
}

void FailingStreamingProvider::set_healthy(bool healthy) {
    SimulatedStreamingProvider::set_healthy(healthy);
    status_ = {healthy_ && connected_, healthy_ && connected_ ? "FAILING_PROVIDER_RECOVERED" : "FAILURE_MODE",
               healthy_ && connected_ ? "" : "Provider intentionally unhealthy"};
}

MarketDataEngine::MarketDataEngine() = default;

void MarketDataEngine::set_historical_provider(std::shared_ptr<HistoricalDataProvider> provider) {
    historical_provider_ = std::move(provider);
}

void MarketDataEngine::set_streaming_provider(std::shared_ptr<StreamingDataProvider> provider) {
    streaming_provider_ = std::move(provider);
    if (streaming_provider_) {
        streaming_provider_->set_event_callback([this](const MarketEvent& event) {
            this->process_event(event);
        });
        streaming_provider_->connect();
    }
}

std::size_t MarketDataEngine::subscribe(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);
    const std::size_t id = next_subscription_id_++;
    subscriptions_[id] = Subscription{symbol, {}};
    if (streaming_provider_) {
        streaming_provider_->subscribe(symbol);
    }
    return id;
}

std::vector<MarketEvent> MarketDataEngine::receive_events(std::size_t subscription_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = subscriptions_.find(subscription_id);
    if (it == subscriptions_.end()) {
        return {};
    }
    std::vector<MarketEvent> events = std::move(it->second.pending_events);
    it->second.pending_events.clear();
    return events;
}

bool MarketDataEngine::process_event(const MarketEvent& event) {
    if (!MarketDataValidator::is_valid(event)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    latest_market_data_[event.symbol] = event;
    for (auto& [id, sub] : subscriptions_) {
        if (sub.symbol == event.symbol) {
            sub.pending_events.push_back(event);
        }
    }
    return true;
}

MarketEvent MarketDataEngine::latest_market_state(const std::string& symbol) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = latest_market_data_.find(symbol);
    if (it == latest_market_data_.end()) {
        return {};
    }
    return it->second;
}

std::vector<MarketEvent> MarketDataEngine::load_history(const std::string& symbol,
                                                       long long start_ms,
                                                       long long end_ms) const {
    if (!historical_provider_) {
        return {};
    }
    return historical_provider_->load_history(symbol, start_ms, end_ms);
}

ProviderStatus MarketDataEngine::provider_status() const {
    ProviderStatus status{true, "OK", ""};
    if (historical_provider_) {
        const auto hist = historical_provider_->connection_status();
        if (!hist.healthy) {
            status = hist;
        }
    }
    if (streaming_provider_) {
        const auto stream = streaming_provider_->connection_status();
        if (!stream.healthy) {
            status = stream;
        }
    }
    return status;
}

}  // namespace trading::core
