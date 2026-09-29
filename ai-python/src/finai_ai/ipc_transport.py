import json
import socket
import threading
import time
from typing import Optional


class LocalSocketServer:
    """Simple local TCP server used as the hot-path IPC bridge between Python and the C++ core."""

    def __init__(self, host: str = "127.0.0.1", port: int = 9001, timeout: float = 2.0):
        self.host = host
        self.port = port
        self.timeout = timeout
        self._socket = None
        self._thread = None
        self.received_payloads: list[str] = []

    def start(self):
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._socket.bind((self.host, self.port))
        self._socket.listen(5)
        self._socket.settimeout(self.timeout)

        self._thread = threading.Thread(target=self._run, daemon=True)
        self._thread.start()
        return self

    def _run(self):
        try:
            conn, _ = self._socket.accept()
            conn.settimeout(self.timeout)
            payload = conn.recv(4096)
            if payload:
                self.received_payloads.append(payload.decode("utf-8"))
                conn.sendall(b"ACK")
            conn.close()
        except socket.timeout:
            pass
        finally:
            if self._socket is not None:
                self._socket.close()

    def wait_for_message(self, timeout: float = 2.0) -> Optional[str]:
        deadline = time.time() + timeout
        while time.time() < deadline:
            if self.received_payloads:
                return self.received_payloads[-1]
            time.sleep(0.02)
        return None

    def stop(self):
        if self._socket is not None:
            try:
                self._socket.close()
            except OSError:
                pass
        if self._thread is not None and self._thread.is_alive():
            self._thread.join(timeout=1)


class LocalSocketClient:
    """Small client used by the AI layer to send a serialized proposal to the core over localhost."""

    def __init__(self, host: str = "127.0.0.1", port: int = 9001, timeout: float = 2.0):
        self.host = host
        self.port = port
        self.timeout = timeout

    def send(self, payload: dict) -> str:
        message = json.dumps(payload).encode("utf-8")
        with socket.create_connection((self.host, self.port), timeout=self.timeout) as sock:
            sock.sendall(message)
            response = sock.recv(4096).decode("utf-8")
            return response
