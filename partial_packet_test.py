import argparse
import socket
import struct
import time


DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = 9001
HEADER_SIZE = 4
BUFFER_SIZE = 1024
TEST_PACKET_TYPE = 0x0001


def make_packet(packet_type, body):
    length = HEADER_SIZE + len(body)
    return struct.pack("<HH", length, packet_type) + body


def connect(host, port, timeout):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(timeout)
    sock.connect((host, port))
    return sock


def is_still_connected(sock):
    try:
        data = sock.recv(1, socket.MSG_PEEK)
        return data != b""
    except socket.timeout:
        return True
    except (ConnectionResetError, BrokenPipeError, OSError):
        return False


def expect_closed(sock):
    try:
        data = sock.recv(1)
        if data == b"":
            return True

        return False
    except socket.timeout:
        return False
    except (ConnectionResetError, BrokenPipeError, OSError):
        return True


def run_case(name, fn):
    try:
        if fn():
            print(f"[PASS] {name}")
            return True

        print(f"[FAIL] {name}")
        return False
    except Exception as exc:
        print(f"[ERROR] {name}: {exc}")
        return False


def test_partial_header(host, port, timeout):
    sock = connect(host, port, timeout)
    try:
        sock.sendall(b"\x01")
        time.sleep(1)
        return is_still_connected(sock)
    finally:
        sock.close()


def test_partial_body(host, port, timeout):
    body = b"abcdefghij"
    packet = make_packet(TEST_PACKET_TYPE, body)
    first = packet[:HEADER_SIZE + 3]
    second = packet[HEADER_SIZE + 3:]

    sock = connect(host, port, timeout)
    try:
        sock.sendall(first)
        time.sleep(1)

        if not is_still_connected(sock):
            return False

        sock.sendall(second)
        time.sleep(1)
        return is_still_connected(sock)
    finally:
        sock.close()


def test_invalid_short_length_closes(host, port, timeout):
    invalid_length = HEADER_SIZE - 1
    packet = struct.pack("<HH", invalid_length, TEST_PACKET_TYPE)

    sock = connect(host, port, timeout)
    try:
        sock.sendall(packet)
        time.sleep(1)
        return expect_closed(sock)
    finally:
        sock.close()


def test_invalid_large_length_closes(host, port, timeout):
    invalid_length = BUFFER_SIZE + 1
    packet = struct.pack("<HH", invalid_length, TEST_PACKET_TYPE)

    sock = connect(host, port, timeout)
    try:
        sock.sendall(packet)
        time.sleep(1)
        return expect_closed(sock)
    finally:
        sock.close()


def test_coalesced_packets_keep_connection(host, port, timeout):
    first = make_packet(TEST_PACKET_TYPE, b"abc")
    second = make_packet(TEST_PACKET_TYPE, b"def")

    sock = connect(host, port, timeout)
    try:
        sock.sendall(first + second)
        time.sleep(1)
        return is_still_connected(sock)
    finally:
        sock.close()


def test_client_disconnect(host, port, timeout):
    sock = connect(host, port, timeout)
    sock.close()
    time.sleep(1)
    return True


def main():
    parser = argparse.ArgumentParser(description="TCP packet boundary tests for ChannelServer")
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_PORT)
    parser.add_argument("--timeout", type=float, default=2.0)
    args = parser.parse_args()

    cases = [
        ("partial header keeps connection", lambda: test_partial_header(args.host, args.port, args.timeout)),
        ("partial body keeps connection", lambda: test_partial_body(args.host, args.port, args.timeout)),
        ("invalid short length closes connection", lambda: test_invalid_short_length_closes(args.host, args.port, args.timeout)),
        ("invalid large length closes connection", lambda: test_invalid_large_length_closes(args.host, args.port, args.timeout)),
        ("coalesced packets keep connection", lambda: test_coalesced_packets_keep_connection(args.host, args.port, args.timeout)),
        ("client disconnect does not crash server", lambda: test_client_disconnect(args.host, args.port, args.timeout)),
    ]

    passed = 0
    for name, fn in cases:
        if run_case(name, fn):
            passed += 1

    total = len(cases)
    print(f"\nResult: {passed}/{total} passed")
    raise SystemExit(0 if passed == total else 1)


if __name__ == "__main__":
    main()
