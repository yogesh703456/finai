#pragma once

#include <string>

namespace trading::core {

struct TransportMessage {
    std::string payload;
    std::string response;
};

inline TransportMessage receive_local_message(const std::string& payload) {
    TransportMessage message{};
    message.payload = payload;
    message.response = "ACK";
    return message;
}

}  // namespace trading::core
