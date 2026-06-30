import argparse
import socket
import struct
import time
from concurrent.futures import ThreadPoolExecutor, as_completed

PKT_CHANNEL_AUTH = 0x0009
HEADER_SIZE = 4


def make_body(*fields):
    body = b""
    for field in fields:
        data = str(field).encode("utf-8")
        body += struct.pack("<H", len(data))
        body += data
    return body


def make_packet(packet_type, body):
    total_length = HEADER_SIZE + len(body)
    return struct.pack("<HH", total_length, packet_type) + body


def parse_fields(body):
    fields = []
    offset = 0

    while offset + 2 <= len(body):
        field_len = struct.unpack_from("<H", body, offset)[0]
        offset += 2

        if offset + field_len > len(body):
            break

        fields.append(body[offset:offset + field_len].decode("utf-8", errors="replace"))
        offset += field_len

    return fields


def parse_packets(buffer):
    packets = []
    offset = 0

    while offset + HEADER_SIZE <= len(buffer):
        packet_length, packet_type = struct.unpack_from("<HH", buffer, offset)

        if packet_length < HEADER_SIZE:
            break

        if offset + packet_length > len(buffer):
            break

        body = buffer[offset + HEADER_SIZE:offset + packet_length]
        packets.append((packet_type, body))
        offset += packet_length

    return packets


def auth_once(host, port, character_id, timeout):
    packet = make_packet(PKT_CHANNEL_AUTH, make_body(str(character_id)))
    received = b""

    start = time.perf_counter()

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.settimeout(timeout)
            sock.connect((host, port))
            sock.sendall(packet)

            while True:
                try:
                    chunk = sock.recv(4096)
                    if chunk == b"":
                        break

                    received += chunk
                    packets = parse_packets(received)

                    for packet_type, body in packets:
                        if packet_type != PKT_CHANNEL_AUTH:
                            continue

                        fields = parse_fields(body)
                        if len(fields) == 0:
                            continue

                        if fields[0] in ("ok", "nok"):
                            elapsed_ms = (time.perf_counter() - start) * 1000
                            return {
                                "character_id": character_id,
                                "success": fields[0] == "ok",
                                "result": fields[0],
                                "name": fields[1] if len(fields) > 1 else None,
                                "elapsed_ms": elapsed_ms,
                                "received_bytes": len(received),
                                "received_packets": len(packets),
                                "error": "",
                            }

                except socket.timeout:
                    break

        elapsed_ms = (time.perf_counter() - start) * 1000
        packets = parse_packets(received)

        return {
            "character_id": character_id,
            "success": False,
            "result": "no_auth_response",
            "name": None,
            "elapsed_ms": elapsed_ms,
            "received_bytes": len(received),
            "received_packets": len(packets),
            "error": "",
        }

    except Exception as e:
        elapsed_ms = (time.perf_counter() - start) * 1000
        return {
            "character_id": character_id,
            "success": False,
            "result": "exception",
            "name": None,
            "elapsed_ms": elapsed_ms,
            "received_bytes": len(received),
            "received_packets": 0,
            "error": repr(e),
        }


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9001)
    parser.add_argument("--start-character-id", type=int, default=900000)
    parser.add_argument("--clients", type=int, default=10)
    parser.add_argument("--timeout", type=float, default=3.0)
    parser.add_argument("--verbose", action="store_true")
    args = parser.parse_args()

    character_ids = [
        args.start_character_id + i
        for i in range(args.clients)
    ]

    print(f"target={args.host}:{args.port}")
    print(f"start_character_id={args.start_character_id}, clients={args.clients}, timeout={args.timeout}")

    started = time.perf_counter()
    results = []

    with ThreadPoolExecutor(max_workers=args.clients) as executor:
        futures = [
            executor.submit(auth_once, args.host, args.port, character_id, args.timeout)
            for character_id in character_ids
        ]

        for future in as_completed(futures):
            results.append(future.result())

    elapsed_sec = time.perf_counter() - started

    success_results = [result for result in results if result["success"]]
    failed_results = [result for result in results if not result["success"]]

    elapsed_values = [result["elapsed_ms"] for result in results]
    packet_values = [result["received_packets"] for result in results]
    byte_values = [result["received_bytes"] for result in results]

    print(f"success={len(success_results)}")
    print(f"failed={len(failed_results)}")
    print(f"elapsed_sec={elapsed_sec:.3f}")

    if elapsed_values:
        print(f"auth_ms_min={min(elapsed_values):.3f}")
        print(f"auth_ms_avg={sum(elapsed_values) / len(elapsed_values):.3f}")
        print(f"auth_ms_max={max(elapsed_values):.3f}")

    if packet_values:
        print(f"received_packets_avg={sum(packet_values) / len(packet_values):.3f}")
        print(f"received_packets_max={max(packet_values)}")

    if byte_values:
        print(f"received_bytes_avg={sum(byte_values) / len(byte_values):.3f}")
        print(f"received_bytes_max={max(byte_values)}")

    if args.verbose or failed_results:
        for result in sorted(results, key=lambda item: item["character_id"]):
            status = "PASS" if result["success"] else "FAIL"
            print(
                f"[{status}] character_id={result['character_id']} "
                f"result={result['result']} "
                f"name={result['name']} "
                f"elapsed_ms={result['elapsed_ms']:.3f} "
                f"packets={result['received_packets']} "
                f"bytes={result['received_bytes']} "
                f"error={result['error']}"
            )

    if failed_results:
        raise SystemExit(1)


if __name__ == "__main__":
    main()