#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace trading::core {

enum class AssetType {
    Unknown = 0,
    Equity,
    Forex,
    Crypto,
    Commodity,
    Index,
};

struct MarketEvent {
    std::string symbol;
    std::string exchange;
    AssetType asset_type = AssetType::Unknown;
    long long timestamp_ms = 0;
    double open = 0.0;
    double high = 0.0;
    double low = 0.0;
    double close = 0.0;
    double volume = 0.0;
};

struct ProviderStatus {
    bool healthy = true;
    std::string status = "OK";
    std::string last_error;
};

class MarketDataProvider {
public:
    virtual ~MarketDataProvider() = default;

    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual ProviderStatus connection_status() const = 0;
};

class HistoricalDataProvider : public virtual MarketDataProvider {
public:
    virtual ~HistoricalDataProvider() override = default;

    virtual std::vector<MarketEvent> load_history(const std::string& symbol,
                                                 long long start_ms,
                                                 long long end_ms) = 0;
};

class StreamingDataProvider : public virtual MarketDataProvider {
public:
    virtual ~StreamingDataProvider() override = default;

    virtual bool subscribe(const std::string& symbol) = 0;
    virtual bool unsubscribe(const std::string& symbol) = 0;
    virtual void publish(const MarketEvent& event) = 0;
    virtual void set_event_callback(std::function<void(const MarketEvent&)> callback) {
        callback_ = std::move(callback);
    }

protected:
    std::function<void(const MarketEvent&)> callback_;
};

class MarketDataValidator {
public:
    static bool is_valid(const MarketEvent& event);
    static AssetType parse_asset_type(const std::string& value);
    static std::string asset_type_to_string(AssetType value);
};

class CsvHistoricalProvider final : public HistoricalDataProvider {
public:
    explicit CsvHistoricalProvider(std::string csv_path);

    bool connect() override;
    void disconnect() override;
    ProviderStatus connection_status() const override;
    std::vector<MarketEvent> load_history(const std::string& symbol,
                                         long long start_ms,
                                         long long end_ms) override;

protected:
    std::string csv_path_;
    bool connected_ = false;
    ProviderStatus status_{true, "OK", ""};
};

class SimulatedStreamingProvider : public StreamingDataProvider {
public:
    explicit SimulatedStreamingProvider(std::string exchange_name);

    bool connect() override;
    void disconnect() override;
    ProviderStatus connection_status() const override;
    bool subscribe(const std::string& symbol) override;
    bool unsubscribe(const std::string& symbol) override;
    void publish(const MarketEvent& event) override;

    virtual void set_healthy(bool healthy);

protected:
    std::string exchange_name_;
    bool connected_ = false;
    bool healthy_ = true;
    std::vector<std::string> subscriptions_;
    mutable std::mutex mutex_;
    ProviderStatus status_{true, "SIMULATED_STREAMING_PROVIDER_READY", ""};
};

class FailingStreamingProvider final : public SimulatedStreamingProvider {
public:
    FailingStreamingProvider();

    ProviderStatus connection_status() const override;
    void set_healthy(bool healthy) override;
};

class MarketDataEngine {
public:
    MarketDataEngine();

    void set_historical_provider(std::shared_ptr<HistoricalDataProvider> provider);
    void set_streaming_provider(std::shared_ptr<StreamingDataProvider> provider);

    std::size_t subscribe(const std::string& symbol);
    std::vector<MarketEvent> receive_events(std::size_t subscription_id);
    bool process_event(const MarketEvent& event);
    MarketEvent latest_market_state(const std::string& symbol) const;
    std::vector<MarketEvent> load_history(const std::string& symbol,
                                         long long start_ms,
                                         long long end_ms) const;
    ProviderStatus provider_status() const;

private:
    struct Subscription {
        std::string symbol;
        std::vector<MarketEvent> pending_events;
    };

    std::shared_ptr<HistoricalDataProvider> historical_provider_;
    std::shared_ptr<StreamingDataProvider> streaming_provider_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, MarketEvent> latest_market_data_;
    std::unordered_map<std::size_t, Subscription> subscriptions_;
    std::size_t next_subscription_id_ = 0;
};

}  // namespace trading::core
