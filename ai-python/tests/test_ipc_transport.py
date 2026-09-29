from finai_ai.ipc_client import ProposalIPCClient
from finai_ai.ipc_transport import LocalSocketClient, LocalSocketServer


def test_local_socket_round_trip():
    server = LocalSocketServer(port=9101)
    server.start()

    payload = ProposalIPCClient().build_proposal("MSFT", "SELL", 0.76, "Risk-off move")
    response = LocalSocketClient(port=9101).send(payload)

    assert response == "ACK"
    assert server.wait_for_message(timeout=2.0) is not None

    server.stop()
