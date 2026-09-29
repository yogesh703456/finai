cmake_minimum_required(VERSION 3.25)
project(FinAITradingCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(trading_core
    src/risk/risk_engine.cpp
    src/paper_trading/paper_trading_engine.cpp
    src/execution/execution_engine.cpp
    src/portfolio/portfolio_state.cpp
    src/market/market_loop.cpp
    src/market/market_data_engine.cpp
)

target_include_directories(trading_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(finai_core src/main.cpp)
target_link_libraries(finai_core PRIVATE trading_core)

enable_testing()
add_executable(risk_engine_smoke tests/risk_engine_smoke.cpp)
target_link_libraries(risk_engine_smoke PRIVATE trading_core)
add_test(NAME risk_engine_smoke COMMAND risk_engine_smoke)

add_executable(paper_trading_smoke tests/paper_trading_smoke.cpp)
target_link_libraries(paper_trading_smoke PRIVATE trading_core)
add_test(NAME paper_trading_smoke COMMAND paper_trading_smoke)

add_executable(ipc_bridge_smoke tests/ipc_bridge_smoke.cpp)
target_link_libraries(ipc_bridge_smoke PRIVATE trading_core)
add_test(NAME ipc_bridge_smoke COMMAND ipc_bridge_smoke)

add_executable(ipc_transport_smoke tests/ipc_transport_smoke.cpp)
target_link_libraries(ipc_transport_smoke PRIVATE trading_core)
add_test(NAME ipc_transport_smoke COMMAND ipc_transport_smoke)

add_executable(execution_engine_smoke tests/execution_engine_smoke.cpp)
target_link_libraries(execution_engine_smoke PRIVATE trading_core)
add_test(NAME execution_engine_smoke COMMAND execution_engine_smoke)

add_executable(market_loop_smoke tests/market_loop_smoke.cpp)
target_link_libraries(market_loop_smoke PRIVATE trading_core)
add_test(NAME market_loop_smoke COMMAND market_loop_smoke)

add_executable(market_data_engine_smoke tests/market_data_engine_smoke.cpp)
target_link_libraries(market_data_engine_smoke PRIVATE trading_core)
add_test(NAME market_data_engine_smoke COMMAND market_data_engine_smoke)

add_executable(market_data_performance tests/market_data_performance.cpp)
target_link_libraries(market_data_performance PRIVATE trading_core psapi)
