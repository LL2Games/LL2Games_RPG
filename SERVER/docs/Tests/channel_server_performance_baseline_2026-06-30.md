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