# 채널 서버 성능/수신 경계 테스트 기준 측정

Related issue: #99

## 환경
- Branch: 99-채널-서버-성능-테스트-기준-측정
- Target: ChannelServer
- Host: 127.0.0.1
- Port: 9001

## 테스트 날짜
- 2026-06-30

## 테스트 파일
- `partial_packet_test.py`

## 실행 명령

### 전체 테스트 명령어
```zsh
python3 partial_packet_test.py --host 127.0.0.1 --port 9001
```

### 개별 테스트 명령어
```zsh
python3 partial_packet_test.py --host 127.0.0.1 --port 9001 --case partial_header
python3 partial_packet_test.py --host 127.0.0.1 --port 9001 --case partial_body
python3 partial_packet_test.py --host 127.0.0.1 --port 9001 --case invalid_short_length
python3 partial_packet_test.py --host 127.0.0.1 --port 9001 --case invalid_large_length
python3 partial_packet_test.py --host 127.0.0.1 --port 9001 --case coalesced_packets
```

## 실행 결과

```text
target=127.0.0.1:9001
Run ChannelServer first, then execute this script.
[PASS] partial_header: expected open, actual open
[PASS] partial_body: expected open, actual open
[PASS] invalid_short_length: expected closed, actual closed
[PASS] invalid_large_length: expected closed, actual closed
[PASS] coalesced_packets: expected open, actual open
5/5 passed
```

## 테스트 결과

### partial header
- 결과: PASS
- 관찰: header 일부만 전송한 상태에서 서버가 연결을 유지함
- 의미: 불완전한 TCP 수신에 대해 즉시 disconnect하지 않고 추가 데이터를 기다림

### partial body
- 결과: PASS
- 관찰: header와 body 일부만 전송한 상태에서 서버가 연결을 유지함
- 의미: body가 모두 도착하기 전까지 추가 데이터를 기다림

### invalid short length
- 결과: PASS
- 관찰: 패킷 길이가 header 크기보다 작은 경우 서버가 연결을 종료함
- 의미: 비정상 length 값을 InvalidPacket으로 처리함

### invalid large length
- 결과: PASS
- 관찰: 패킷 길이가 BUFFER_SIZE보다 큰 경우 서버가 연결을 종료함
- 의미: 허용 범위를 초과한 패킷을 방어함

### coalesced packets
- 결과: PASS
- 관찰: 두 개의 패킷을 한 번에 전송했을 때 서버가 연결을 유지함
- 의미: TCP coalescing 상황에서 서버가 즉시 오류 처리하지 않음
- 비고: 현재 테스트는 연결 유지 여부만 확인하며, 각 패킷의 개별 Dispatch 여부는 서버 로그 보강 후 추가 확인 필요

## 테스트 방식
- Python socket 스크립트를 사용해 외부 클라이언트 관점에서 TCP 패킷 경계 상황을 재현
- 서버 코드를 직접 호출하지 않고 실제 TCP 연결을 통해 partial/invalid/coalesced packet을 전송

## 남은 테스트
- coalesced packet 개별 Dispatch 로그 확인
- 동시 접속 기준 측정
- 패킷 처리량 기준 측정
- 평균/최대 응답 시간 측정


<br></br>
# 동시 접속 테스트


## 테스트 파일
- channel_load_test.py

## 테스트 목적
- 여러 클라이언트가 동시에 ChannelServer에 접속했을 때 서버가 안정적으로 accept/session 생성/disconnect 처리를 수행하는지 확인
- TCP 경계 테스트 이후 기본 부하 상황에서 서버가 크래시 없이 유지되는지 확인

## 실행 명령

```zsh
python3 channel_load_test.py --host 127.0.0.1 --port 9001 --clients 10
python3 channel_load_test.py --host 127.0.0.1 --port 9001 --clients 50
python3 channel_load_test.py --host 127.0.0.1 --port 9001 --clients 100
python3 channel_load_test.py --host 127.0.0.1 --port 9001 --clients 10000 --timeout 5
python3 channel_load_test.py --host 127.0.0.1 --port 9001 --clients 20000 --timeout 10
python3 channel_load_test.py --host 127.0.0.1 --port 9001 --clients 50000 --timeout 10
```

## 실행 결과

### clients 10

```text
target=127.0.0.1:9001
clients=10, timeout=2.0, hold_time=0.2
success=10
failed=0
elapsed_sec=0.214
connect_ms_min=0.268
connect_ms_avg=0.479
connect_ms_max=0.978
```

### clients 50

```text
target=127.0.0.1:9001
clients=50, timeout=2.0, hold_time=0.2
success=50
failed=0
elapsed_sec=0.217
connect_ms_min=0.172
connect_ms_avg=0.499
connect_ms_max=1.470
```

### clients 100

```text
target=127.0.0.1:9001
clients=100, timeout=2.0, hold_time=0.2
success=100
failed=0
elapsed_sec=0.260
connect_ms_min=0.168
connect_ms_avg=10.021
connect_ms_max=29.110
```

## 테스트 결과

### clients 10
- 결과: PASS
- 관찰: 10개 클라이언트가 모두 접속 성공했고 실패 없음
- 의미: 소규모 동시 접속 상황에서 연결 수락 및 종료 흐름이 정상 동작함

### clients 50
- 결과: PASS
- 관찰: 50개 클라이언트가 모두 접속 성공했고 실패 없음
- 의미: 중간 규모 동시 접속 상황에서도 accept/session 처리 흐름이 유지됨

### clients 100
- 결과: PASS
- 관찰: 100개 클라이언트가 모두 접속 성공했고 실패 없음. 평균 접속 시간은 10.021ms, 최대 접속 시간은 29.110ms로 증가함
- 의미: 100개 동시 접속까지 안정성은 확인했으며, 접속 수 증가에 따른 accept 지연이 baseline 수치로 관찰됨

### clients 10000
- 결과: PASS
- 관찰: 10000개 동시 접속 시도 모두 성공, 실패 없음
- 의미: 로컬 환경 기준 대량 접속 accept/disconnect 흐름이 안정적으로 처리됨

### clients 20000
- 결과: PASS
- 관찰: 20000개 동시 접속 시도 모두 성공, 실패 없음
- 의미: 로컬 환경 기준 20000개 접속까지 실패 없이 처리됨

### clients 50000
- 결과: BLOCKED
- 관찰: 테스트 클라이언트가 Python 스레드를 생성하는 중  RuntimeError: can't start new thread 로 종료됨
- 의미: ChannelServer 한계가 아니라 테스트 클라이언트/OS 스레드 생성 한계에 먼저 도달함

비고: 본 테스트 방식은 clients 수만큼 Python thread를 생성하므로, 50000 단계에서는 서버 부하 한계보다 테스트 클라이언트의 스레드 생성 한계가 먼저 관찰됨

## 테스트 방식
- channel_load_test.py 는  ThreadPoolExecutor 를 사용해 clients 수만큼 동시 접속 작업을 생성
- 각 작업은 하나의 TCP socket을 생성하고 ChannelServer에 접속한 뒤, hold_time 동안 연결을 유지하고 종료
- 따라서 본 테스트는 서버의 패킷 처리 성능이 아니라 대량 TCP 연결 accept/disconnect 안정성을 확인하는 테스트
- 50000 clients 단계에서는 서버 한계보다 Python 테스트 클라이언트의 thread 생성 한계가 먼저 관찰됨

## 남은 테스트
- 패킷 반복 전송 기준 측정
- 처리량 기준 측정
- 평균/최대 응답 시간 측정

<br></br>


# 패킷 반복 전송 테스트

## 테스트 파일
-  channel_packet_burst_test.py 

## 테스트 목적
- 여러 클라이언트가 ChannelServer에 접속한 뒤 짧은 패킷을 반복 전송했을 때 서버가 안정적으로 수신 흐름을 유지하는지 확인
-  recv -> OnBytes -> TryParse  경로에 반복 패킷 부하를 주고 연결 종료/크래시 여부를 확인
- 실제 게임 핸들러 처리량이 아니라 TCP 수신/파싱 경로의 안정성 baseline을 측정

## 실행 명령

```zsh
python3 channel_packet_burst_test.py --host 127.0.0.1 --port 9001 --clients 10 --packets 100
python3 channel_packet_burst_test.py --host 127.0.0.1 --port 9001 --clients 50 --packets 100
python3 channel_packet_burst_test.py --host 127.0.0.1 --port 9001 --clients 500 --packets 1000 --timeout 10
python3 channel_packet_burst_test.py --host 127.0.0.1 --port 9001 --clients 1000 --packets 100 --timeout 10
python3 channel_packet_burst_test.py --host 127.0.0.1 --port 9001 --clients 1000 --packets 500 --timeout 15
python3 channel_packet_burst_test.py --host 127.0.0.1 --port 9001 --clients 1000 --packets 1000 --timeout 30
```

## 실행 결과

### clients 10, packets 100

```text
target=127.0.0.1:9001
clients=10, packets_per_client=100, total_expected=1000, timeout=2.0, delay=0.0
success_clients=10
failed_clients=0
total_sent=1000
elapsed_sec=0.071
packets_per_sec=14026.072
connect_ms_avg=1.678
connect_ms_max=2.751
send_ms_avg=65.028
send_ms_max=68.484
```

### clients 50, packets 100

```text
target=127.0.0.1:9001
clients=50, packets_per_client=100, total_expected=5000, timeout=2.0, delay=0.0
success_clients=50
failed_clients=0
total_sent=5000
elapsed_sec=0.347
packets_per_sec=14390.211
connect_ms_avg=5.355
connect_ms_max=15.628
send_ms_avg=281.236
send_ms_max=311.365
```

### clients 500, packets 1000

```text
target=127.0.0.1:9001
clients=500, packets_per_client=1000, total_expected=500000, timeout=10.0, delay=0.0
success_clients=500
failed_clients=0
total_sent=500000
elapsed_sec=25.637
packets_per_sec=19503.141
connect_ms_avg=34.347
connect_ms_max=148.269
send_ms_avg=21570.690
send_ms_max=23468.261
```

### clients 1000, packets 100

```text
target=127.0.0.1:9001
clients=1000, packets_per_client=100, total_expected=100000, timeout=10.0, delay=0.0
success_clients=1000
failed_clients=0
total_sent=100000
elapsed_sec=6.183
packets_per_sec=16174.264
connect_ms_avg=34.008
connect_ms_max=216.466
send_ms_avg=1448.410
send_ms_max=2263.050
```

### clients 1000, packets 500

```text
target=127.0.0.1:9001
clients=1000, packets_per_client=500, total_expected=500000, timeout=15.0, delay=0.0
success_clients=1000
failed_clients=0
total_sent=500000
elapsed_sec=25.000
packets_per_sec=19999.856
connect_ms_avg=51.927
connect_ms_max=261.606
send_ms_avg=12841.233
send_ms_max=16426.109
```

### clients 1000, packets 1000

```text
target=127.0.0.1:9001
clients=1000, packets_per_client=1000, total_expected=1000000, timeout=30.0, delay=0.0
success_clients=1000
failed_clients=0
total_sent=1000000
elapsed_sec=46.778
packets_per_sec=21377.561
connect_ms_avg=53.508
connect_ms_max=343.508
send_ms_avg=32658.624
send_ms_max=37331.865
```

## 테스트 결과

### clients 10, packets 100
- 결과: PASS
- 관찰: 10개 클라이언트가 총 1000개 패킷을 실패 없이 전송함
- 의미: 소규모 반복 패킷 전송 상황에서 수신/파싱 경로가 안정적으로 유지됨

### clients 50, packets 100
- 결과: PASS
- 관찰: 50개 클라이언트가 총 5000개 패킷을 실패 없이 전송함
- 의미: 클라이언트 수가 증가해도 반복 패킷 전송 중 연결 실패가 발생하지 않음

### clients 500, packets 1000
- 결과: PASS
- 관찰: 500개 클라이언트가 각 1000개 패킷을 전송해 총 500000개 패킷을 실패 없이 전송함
- 의미: 다수 클라이언트의 반복 패킷 전송 상황에서도 TCP 수신/파싱 경로가 안정적으로 유지됨

### clients 1000, packets 100
- 결과: PASS
- 관찰: 1000개 클라이언트가 각 100개 패킷을 전송해 총 100000개 패킷을 실패 없이 전송함
- 의미: 1000개 동시 클라이언트 환경에서도 반복 패킷 전송 중 연결 실패가 발생하지 않음

### clients 1000, packets 500
- 결과: PASS
- 관찰: 1000개 클라이언트가 각 500개 패킷을 전송해 총 500000개 패킷을 실패 없이 전송함
- 의미: 동일한 총 500000개 패킷 기준에서 500 clients/1000 packets와 1000 clients/500 packets 모두 실패 없이 완료됨

### clients 1000, packets 1000
- 결과: PASS
- 관찰: 1000개 클라이언트가 각 1000개 패킷을 전송해 총 1000000개 패킷을 실패 없이 전송함
- 의미: 1000개 동시 클라이언트의 반복 패킷 전송 상황에서 연결 실패 없이 수신/파싱 경로가 유지됨

## 테스트 방식
-  channel_packet_burst_test.py 는 clients 수만큼 Python thread를 생성하고, 각 thread가 하나의 TCP socket을 통해 반복 패킷을 전송
- 각 패킷은 서버의  PacketHeader(length, type)  구조에 맞춰 구성
- 테스트 패킷 타입은 unknown packet을 사용하므로 실제 게임 핸들러 처리량을 측정하지 않음
- 본 테스트는 클라이언트 기준 send 성공 및 연결 안정성을 측정

## 패킷 반복 전송 테스트 결론
- 로컬 환경 기준 1000개 클라이언트가 총 1000000개 패킷을 실패 없이 전송함
- 클라이언트 기준 약 21k packets/sec 수준의 반복 전송이 관찰됨
- TCP 수신/파싱 경로는 반복 패킷 부하 상황에서도 안정적으로 유지됨
- 실제 게임 로직 처리량, handler 처리 완료율, 응답 latency는 별도 테스트가 필요함

## 남은 테스트
- 실제 패킷 타입 기반 handler 부하 테스트
- coalesced packet 개별 Dispatch 로그 확인
- 서버 내부 수신/Dispatch 카운터 로그 보강
- CPU/메모리/FD 사용량 측정
- 장시간 soak test

