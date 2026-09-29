#include "trading_core/ipc_transport.h"

#include <cassert>
#include <string>

int main() {
    const std::string payload = R"({"version":"1.0","symbol":"AAPL","side":"BUY","confidence":0.82,"reason":"Bullish breakout","timestamp_ms":1710000000000})";
    const auto response = trading::core::receive_local_message(payload);

    assert(response.payload == payload);
    assert(response.response == "ACK");

    return 0;
}
