#pragma once

#include <string>

namespace trading::core {

struct SignalProposal {
    std::string version;
    std::string symbol;
    std::string side;
    double confidence = 0.0;
    std::string reason;
    long long timestamp_ms = 0;
};

inline SignalProposal parse_signal_proposal(const std::string& payload) {
    SignalProposal proposal{};

    const std::string version_key = "\"version\":\"";
    const std::string symbol_key = "\"symbol\":\"";
    const std::string side_key = "\"side\":\"";
    const std::string confidence_key = "\"confidence\":";
    const std::string reason_key = "\"reason\":\"";
    const std::string timestamp_key = "\"timestamp_ms\":";

    const auto find_value = [&](const std::string& key, size_t start) {
        const auto pos = payload.find(key, start);
        if (pos == std::string::npos) {
            return std::string{};
        }
        const auto value_start = pos + key.size();
        const auto value_end = payload.find('"', value_start);
        return payload.substr(value_start, value_end - value_start);
    };

    const auto find_double = [&](const std::string& key) {
        const auto pos = payload.find(key);
        if (pos == std::string::npos) {
            return 0.0;
        }
        const auto value_start = pos + key.size();
        const auto value_end = payload.find(',', value_start);
        const auto end = value_end == std::string::npos ? payload.find('}', value_start) : value_end;
        return std::stod(payload.substr(value_start, end - value_start));
    };

    const auto find_long_long = [&](const std::string& key) {
        const auto pos = payload.find(key);
        if (pos == std::string::npos) {
            return 0LL;
        }
        const auto value_start = pos + key.size();
        const auto value_end = payload.find(',', value_start);
        const auto end = value_end == std::string::npos ? payload.find('}', value_start) : value_end;
        return std::stoll(payload.substr(value_start, end - value_start));
    };

    proposal.version = find_value(version_key, 0);
    proposal.symbol = find_value(symbol_key, 0);
    proposal.side = find_value(side_key, 0);
    proposal.confidence = find_double(confidence_key);
    proposal.reason = find_value(reason_key, 0);
    proposal.timestamp_ms = find_long_long(timestamp_key);

    return proposal;
}

inline bool is_valid_signal_proposal(const SignalProposal& proposal) {
    if (proposal.version.empty() || proposal.symbol.empty() || proposal.reason.empty()) {
        return false;
    }

    if (proposal.side != "BUY" && proposal.side != "SELL") {
        return false;
    }

    if (proposal.confidence < 0.0 || proposal.confidence > 1.0) {
        return false;
    }

    if (proposal.timestamp_ms <= 0) {
        return false;
    }

    return true;
}

}  // namespace trading::core
