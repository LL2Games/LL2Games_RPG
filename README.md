# LL2Games RPG Server

> C++17 기반 Linux MMORPG 게임 서버 - 멀티 프로세스 아키텍처

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C++-17-00599C.svg?logo=c%2B%2B)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)](https://www.linux.org/)

---

## 📋 목차

- [프로젝트 개요](#-프로젝트-개요)
- [주요 기능](#-주요-기능)
- [아키텍처](#-아키텍처)
- [기술 스택](#-기술-스택)
- [프로젝트 구조](#-프로젝트-구조)
- [빌드 및 실행](#-빌드-및-실행)
- [시퀀스 다이어그램](#-시퀀스-다이어그램)
- [클래스 다이어그램](#-클래스-다이어그램)
- [코딩 규칙](#-코딩-규칙)

---

## 🎮 프로젝트 개요

LL2Games_RPG는 C++17로 작성된 Linux 기반 MMORPG 게임 서버입니다. 
시스템은 게임의 다양한 측면을 처리하는 전문화된 데몬 프로세스로 구성된 **멀티 서버 아키텍처**를 구현합니다.

### 서버 구성

| 서버 | 역할 |
|------|------|
| **MAIN** | 프로세스 관리자 및 모니터 - 모든 데몬 프로세스 관리 |
| **LOGIN** | 플레이어 인증 및 계정 관리 |
| **WORLD** | 캐릭터 선택, 채널 선택 및 월드 레벨 작업 |
| **CHANNEL** | 핵심 게임플레이 (맵, 플레이어, 몬스터, 전투, 스탯) |
| **CHAT** | 게임 내 채팅 및 커맨드 라우팅 |

### 핵심 특징

- **프로세스 간 통신(IPC)**: POSIX 메시지 큐를 통한 서버 간 통신
- **고성능 네트워킹**: epoll 기반 이벤트 루프를 사용하는 TCP 소켓
- **멀티스레딩**: 커스텀 스레드 풀 구현 (pthread)
- **데이터 영속성**: MySQL (관계형 데이터) + Redis (캐싱 및 실시간 상태)

---

## ✨ 주요 기능

### 인증 및 계정 관리
- 사용자 로그인/로그아웃
- 계정 생성 및 관리
- 세션 관리 (Redis 기반)

### 캐릭터 시스템
- 캐릭터 생성/삭제
- 캐릭터 선택 및 채널 입장
- 스탯 시스템 (STR, DEX, INT, LUK)
- 레벨 및 경험치 관리

### 게임플레이
- 실시간 맵 시스템
- 플레이어 이동 및 위치 동기화
- 몬스터 AI 및 전투 시스템
- 데미지 계산 및 전투 로직

### 채팅 시스템
- 전체 채팅
- 귓속말
- 커맨드 패턴 기반 라우팅

---

## 🏗️ 아키텍처

### 시스템 아키텍처

```
┌─────────────────────────────────────────────────────────┐
│                      MAIN Server                        │
│              (Process Manager & Monitor)                │
└─────────────────────────────────────────────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
┌───────▼────────┐  ┌──────▼──────┐  ┌────────▼────────┐
│  LOGIN Server  │  │ WORLD Server │  │ CHANNEL Server  │
│  (Auth & Acc)  │  │ (Char Select)│  │  (Gameplay)     │
└────────────────┘  └──────────────┘  └─────────────────┘
        │                   │                   │
        └───────────────────┼───────────────────┘
                            │
                    ┌───────▼────────┐
                    │   CHAT Server  │
                    │   (Messaging)  │
                    └────────────────┘
                            │
        ┌───────────────────┴───────────────────┐
        │                                       │
┌───────▼────────┐                    ┌────────▼────────┐
│  MySQL Server  │                    │  Redis Server   │
│  (Persistent)  │                    │    (Cache)      │
└────────────────┘                    └─────────────────┘
```

### 계층 구조

각 서버는 명확한 계층 분리를 따릅니다:

```
┌─────────────────────────────────────┐
│      Packet Handler Layer           │  ← 네트워크 프로토콜
├─────────────────────────────────────┤
│      Service Layer                  │  ← 비즈니스 로직
├─────────────────────────────────────┤
│      Repository Layer               │  ← 데이터 접근
├─────────────────────────────────────┤
│      Database Layer                 │  ← MySQL/Redis
└─────────────────────────────────────┘
```

### 디자인 패턴

- **Command 패턴**: 채팅 커맨드 및 IPC 작업
- **Factory 패턴**: 패킷 생성 (예: `ChannelPacketFactory`)
- **Repository 패턴**: 데이터베이스 접근 (예: `PlayerStatRepository`)
- **Service 패턴**: 비즈니스 로직 계층 (예: `StatService`, `MapService`)
- **Manager 패턴**: 리소스 관리 (예: `PlayerManager`, `MapManager`)
- **Object Pool**: MySQL 및 Redis 커넥션 풀링

---

## 🛠️ 기술 스택

### 언어 및 표준
- **언어**: C++17
- **플랫폼**: Linux
- **컴파일러**: g++ with `-std=c++17 -O2 -Wall -Wextra -Werror`

### 빌드 시스템
- **빌드 도구**: GNU Make (재귀적 makefile)
- **컴파일러 플래그**: `-MMD -MP` (의존성 추적)
- **링커 플래그**: `-pthread` with rpath

### 네트워킹 및 IPC
- **네트워킹**: epoll 기반 이벤트 루프를 사용하는 TCP 소켓
- **IPC**: POSIX 메시지 큐
- **스레딩**: pthread 기반 커스텀 스레드 풀

### 데이터베이스
- **주 DB**: MySQL (libmysqlclient)
  - 영구 저장소 (캐릭터, 계정, 스탯)
  - 커스텀 커넥션 풀 구현
- **캐시**: Redis (hiredis)
  - 실시간 데이터 및 세션 관리

### 라이브러리
- **로깅**: 커스텀 slog 라이브러리 (libslog.so)
- **JSON**: nlohmann/json (헤더 온리 json.hpp)

---

## 📁 프로젝트 구조

### 디렉토리 구성

```
SERVER/
├── bin/          # 컴파일된 데몬 실행 파일 (mainD, loginD, chatD, worldD, channelD)
├── build/        # 빌드 산출물
├── docs/         # UML 다이어그램 및 아키텍처 문서
├── include/      # 서버 타입별로 구성된 헤더 파일
│   ├── COMMON/   # 공유 코드 (패킷 베이스, DB 풀, 스레드 풀, 로깅)
│   ├── MAIN/     # MAIN 서버 헤더
│   ├── LOGIN/    # LOGIN 서버 헤더
│   ├── CHAT/     # CHAT 서버 헤더
│   ├── WORLD/    # WORLD 서버 헤더
│   └── CHANNEL/  # CHANNEL 서버 헤더
├── src/          # 서버 타입별로 구성된 소스 파일
├── obj/          # 컴파일된 오브젝트 파일 (src 구조 미러링)
├── lib/          # 정적 및 공유 라이브러리
│   ├── common/   # 공통 라이브러리
│   ├── mysql/    # MySQL 라이브러리
│   └── slog/     # 로깅 라이브러리
├── logs/         # 데몬별로 구성된 런타임 로그
└── test/         # 테스트 유틸리티 및 IPC 테스트 프로그램
```

### 서버 모듈 구조

각 서버 데몬은 일관된 내부 구성을 따릅니다:

```
SERVER/{include,src}/{SERVER_TYPE}/
├── core/         # 핵심 비즈니스 로직, 서버 생명주기, 도메인 모델
├── db/           # 데이터베이스 접근 계층 (MySQL, Redis)
├── packet/       # 패킷 핸들러, 팩토리, 직렬화
├── ipc/          # 프로세스 간 통신 (메시지 큐)
├── command/      # 커맨드 패턴 구현 (CHAT만 해당)
├── stat/         # 캐릭터 스탯 시스템 (CHANNEL만 해당)
└── util/         # 서버별 유틸리티
```

### 계층별 책임

| 계층 | 책임 | 규칙 |
|------|------|------|
| **Core** | 서버 초기화, 이벤트 루프, 도메인 모델, 비즈니스 로직 | - |
| **Database** | MySQL 커넥션 풀링, Redis 작업, Repository 패턴 | 이 계층 외부에서 SQL 쿼리 금지 |
| **Packet** | 패킷 타입 정의, 핸들러, 직렬화, 네트워크 프로토콜 | 핸들러는 얇게 유지하고 서비스에 위임 |
| **IPC** | 메시지 큐 작업, 서버 간 통신 | 클라이언트 대면 패킷 계층과 분리 |
| **Command** | 커맨드 패턴 구현, 디스패처 | 서버 간 커맨드 라우팅 |

---

## 🚀 빌드 및 실행

### 사전 요구사항

```bash
# 필수 패키지 설치
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    g++ \
    make \
    libmysqlclient-dev \
    libhiredis-dev
```

### 빌드

```bash
# 전체 빌드
cd SERVER
make

# 특정 서버 빌드
make login    # LOGIN 서버 빌드
make chat     # CHAT 서버 빌드
make world    # WORLD 서버 빌드
make channel  # CHANNEL 서버 빌드
make main     # MAIN 서버 빌드

# 공통 라이브러리만 빌드
make common

# 빌드 산출물 정리
make clean    # 오브젝트 파일 제거
make fclean   # 오브젝트 및 바이너리 제거
make re       # 전체 재빌드 (fclean + all)
```

### 실행

```bash
# 서버 실행 (bin 디렉토리에서)
cd SERVER/bin

# MAIN 서버 먼저 실행 (프로세스 관리자)
./mainD

# 개별 서버 실행
./loginD
./worldD
./channelD
./chatD
```

### 출력 구조

- **바이너리**: `SERVER/bin/` - 컴파일된 데몬 실행 파일
- **라이브러리**: `SERVER/lib/` - 정적 및 공유 라이브러리
- **오브젝트**: `SERVER/obj/` - 컴파일된 오브젝트 파일
- **로그**: `SERVER/logs/` - 데몬 타입별로 구성된 서버 로그

---

## 📊 시퀀스 다이어그램

### 로그인 플로우

사용자 인증 및 로그인 프로세스:

[![](https://mermaid.ink/img/pako:eNqNVE1zmzAQ_SsancwMdXFsjOGQQ51JmoljO3VPLZ2OBq2JxiBRAU5cj_97ZfMREbAbDgzafW_f210NexwICtjDKfzJgQdww0goSexzpB4SZEKiacSAZ0UkITJjAUsIz9BMhIyvQG5BnkleZC5JsIHs9qSxO4P5SjiNuso_7lZPs0fCSVhli3eh-On6WjPnoangHILSiJapcAXLQxxedOO9NTXqQbAtyaDdVpdsVW758P33bHF3P0c9Rk2UvBiag26nSykCSNNS3zjruDE81aAE5a5XC-rEBrTil4PVWi4jvc6OG3vQapdxVVVfiFckm03XBdur0yOtUk85yB26-dIBbTWT5sFxep_XhEUFnsIF3dJYVF42tCroRfi_Pd5BNmfBRnVpvDEu--MKz0kMZxUa12cFnC42b0s10c9jgV_GO3p9kyriN0gTwVNAi4cCClFaLhLd1qP5iPxcNPR9fM-3JGIUBRKoQjESpT7-uKF57YhTfQfamtr37X2yqIpNHEpGsZfJHEwcg4zJ8Yj3R5aPs2dQg8ae-qREbnzs84PiqL_HDyHiiiZFHj5jb636UKc8oUql_AXWEOUV5FTkStMbXJ1KYG-PX9XJnvTtsWPZ44kztoa2beId9kZuf-gOx66lIoOr4cgaH0z89yRq9SfOyFWP49oDSyGcwz_kLs1l?type=png)](https://mermaid.live/edit#pako:eNqNVE1zmzAQ_SsancwMdXFsjOGQQ51JmoljO3VPLZ2OBq2JxiBRAU5cj_97ZfMREbAbDgzafW_f210NexwICtjDKfzJgQdww0goSexzpB4SZEKiacSAZ0UkITJjAUsIz9BMhIyvQG5BnkleZC5JsIHs9qSxO4P5SjiNuso_7lZPs0fCSVhli3eh-On6WjPnoangHILSiJapcAXLQxxedOO9NTXqQbAtyaDdVpdsVW758P33bHF3P0c9Rk2UvBiag26nSykCSNNS3zjruDE81aAE5a5XC-rEBrTil4PVWi4jvc6OG3vQapdxVVVfiFckm03XBdur0yOtUk85yB26-dIBbTWT5sFxep_XhEUFnsIF3dJYVF42tCroRfi_Pd5BNmfBRnVpvDEu--MKz0kMZxUa12cFnC42b0s10c9jgV_GO3p9kyriN0gTwVNAi4cCClFaLhLd1qP5iPxcNPR9fM-3JGIUBRKoQjESpT7-uKF57YhTfQfamtr37X2yqIpNHEpGsZfJHEwcg4zJ8Yj3R5aPs2dQg8ae-qREbnzs84PiqL_HDyHiiiZFHj5jb636UKc8oUql_AXWEOUV5FTkStMbXJ1KYG-PX9XJnvTtsWPZ44kztoa2beId9kZuf-gOx66lIoOr4cgaH0z89yRq9SfOyFWP49oDSyGcwz_kLs1l)

### 캐릭터 선택 및 채널 입장

캐릭터 선택 후 게임 채널 입장 프로세스:

[![](https://mermaid.ink/img/pako:eNqtVV1v2jAU_SuWn6iUoiTAYHmohFK2odGPAVKllQp59gWsJjZ1nHYM8d_nJAQCSUc7lYeQXN9z7r3nOM4aU8kAeziCpxgEhUtO5oqEE4HMj1AtFfIDDkJnkSVRmlO-JEKjO6kCNgL1DOrVxSjiUpRX_QVRhhxUAucUKjOEgOCKCDKv4r9ajZ4CX5ocqk2JWymDctIQGI-K7WfXLHJ-cVGYwENbsiylsLLPS4fxkICXg_FqM3a204s_Ew0V01dX3jLefh9P-9f98fTuZji4RDVCqYyFnvKcuJheQo9AdzMAZyehx8p76CvoXXDAI12m2I1Vbdtx1FSpcMdDP2JQK0Tz7ChDV6SeV_a5A6LAdPlK6ZI49wnMsVK0m_01HjIwgxODHUuXmpfoLdjNY-3QtIx6mrR29kbHR71Bzx9P_W_dYdcf94aollK83bmBJGwfjcOQqNURxwdZl1QqGMCIJv_t3h5cUb3wxpo3ktAFnBjw9A6IYkrN_Yd5fmzb--2-vu4NUrOTE-5ffhdOwKSFwIi8jZbRxblKJ-dhrIL8CxcM_YJIoy1xJa4013a9L2YS1fjSQkup8v1_KHWppfcInQhmofu8wMOB5IU6RU5s4bniDHtaxWDhEFRIkke8TmATrBcQwgR75pYR9TjBE7ExGPPd-CllmMOUjOcL7M1IEJmneGk2b_6J3EWVaRmUn5yZ2HPbKQf21vg39pxmu-7aTtuxG0230XBaroVX2Gt26q7rOk27bXc-Ne3PrY2F_6RV7Xqn3dr8BSjomkI?type=png)](https://mermaid.live/edit#pako:eNqtVV1v2jAU_SuWn6iUoiTAYHmohFK2odGPAVKllQp59gWsJjZ1nHYM8d_nJAQCSUc7lYeQXN9z7r3nOM4aU8kAeziCpxgEhUtO5oqEE4HMj1AtFfIDDkJnkSVRmlO-JEKjO6kCNgL1DOrVxSjiUpRX_QVRhhxUAucUKjOEgOCKCDKv4r9ajZ4CX5ocqk2JWymDctIQGI-K7WfXLHJ-cVGYwENbsiylsLLPS4fxkICXg_FqM3a204s_Ew0V01dX3jLefh9P-9f98fTuZji4RDVCqYyFnvKcuJheQo9AdzMAZyehx8p76CvoXXDAI12m2I1Vbdtx1FSpcMdDP2JQK0Tz7ChDV6SeV_a5A6LAdPlK6ZI49wnMsVK0m_01HjIwgxODHUuXmpfoLdjNY-3QtIx6mrR29kbHR71Bzx9P_W_dYdcf94aollK83bmBJGwfjcOQqNURxwdZl1QqGMCIJv_t3h5cUb3wxpo3ktAFnBjw9A6IYkrN_Yd5fmzb--2-vu4NUrOTE-5ffhdOwKSFwIi8jZbRxblKJ-dhrIL8CxcM_YJIoy1xJa4013a9L2YS1fjSQkup8v1_KHWppfcInQhmofu8wMOB5IU6RU5s4bniDHtaxWDhEFRIkke8TmATrBcQwgR75pYR9TjBE7ExGPPd-CllmMOUjOcL7M1IEJmneGk2b_6J3EWVaRmUn5yZ2HPbKQf21vg39pxmu-7aTtuxG0230XBaroVX2Gt26q7rOk27bXc-Ne3PrY2F_6RV7Xqn3dr8BSjomkI)

### 채널 서버 입장 및 맵 진입

채널 서버 접속 후 맵 진입 프로세스:

[![](https://mermaid.ink/img/pako:eNqNVW1v2jAQ_iuWP8FEEa-jsyYkxtBarbyodPuwMSEvPsCDOJnj0LKq_31OTCAOhpYPIT77ubvn7vHlGXsBA0xwBH9jEB585nQpqT8TSP-opwKJ-hsOQhlLSKXiHg-pUKi_okLAZgpyC_LCdhTxQJzuTzZ0BzJBcw_ObQ-poEuX9yENL-3dikhR4fJ7D4xHeUrmaSxX3a7FiqB-oFeeQirICO3P54_lYSlbggQ8FipQWrDyoax8SxU4S3Qunb3fydeHef-mNxoN7ua9bw83qOStqJxzVtEprkGUC-mlMO0nR5ug7yD5YmcA5nxu-8oRdUs3nFn5Ff1b3SToLqDMmLL8itwd7bdMxZy_gEIe9VbAEKOKuhGFLPoSjqFQ8PuP7qQL6GAcC64vxDxU8qM52zVABpcYXCzOXq8E9VhWmzD9c5fGkrdlOvU41fcUEBfInxuXEfJp6II6qEax5-n3M_ysNM7wK3qcgnIRPMXtezsFwcbrUlHb5TffiMHoYXA_H_YmqKR5H9V2EvE4NlJFjaWRiI06VKA4ZI5r4yqbMua65wylMtKDc6klC088UlwsTz3bIyrn2sExd_zdSZ-KWZ6nfsx3LAZCHfqTjg-rVbnDRahWbzINM6XdaX6vYm4FV9OQPophoG06rjNMJodPUk8Pj0ZqnxOCJNdLiERAyDe-0caZ0WvCf7tED2KroJ-JbpJx9MvSqt0cu9G5PTsYruCl5AwTJWOoYB-kT5Mlfk6AM6xW4MMME_3KqFzP8Ey8aIz-qP0IAj-DySBerjBZ0E2kV3Gok8u-6Qer1FxA9oNYKEya9dQHJs_4CZN6q1Nt1Oqdeq3ZajSb9XajgneYtK6rjUaj3qp1atfvW7UP7ZcK_pdGrVWvO-2X_5yF0rQ?type=png)](https://mermaid.live/edit#pako:eNqNVW1v2jAQ_iuWP8FEEa-jsyYkxtBarbyodPuwMSEvPsCDOJnj0LKq_31OTCAOhpYPIT77ubvn7vHlGXsBA0xwBH9jEB585nQpqT8TSP-opwKJ-hsOQhlLSKXiHg-pUKi_okLAZgpyC_LCdhTxQJzuTzZ0BzJBcw_ObQ-poEuX9yENL-3dikhR4fJ7D4xHeUrmaSxX3a7FiqB-oFeeQirICO3P54_lYSlbggQ8FipQWrDyoax8SxU4S3Qunb3fydeHef-mNxoN7ua9bw83qOStqJxzVtEprkGUC-mlMO0nR5ug7yD5YmcA5nxu-8oRdUs3nFn5Ff1b3SToLqDMmLL8itwd7bdMxZy_gEIe9VbAEKOKuhGFLPoSjqFQ8PuP7qQL6GAcC64vxDxU8qM52zVABpcYXCzOXq8E9VhWmzD9c5fGkrdlOvU41fcUEBfInxuXEfJp6II6qEax5-n3M_ysNM7wK3qcgnIRPMXtezsFwcbrUlHb5TffiMHoYXA_H_YmqKR5H9V2EvE4NlJFjaWRiI06VKA4ZI5r4yqbMua65wylMtKDc6klC088UlwsTz3bIyrn2sExd_zdSZ-KWZ6nfsx3LAZCHfqTjg-rVbnDRahWbzINM6XdaX6vYm4FV9OQPophoG06rjNMJodPUk8Pj0ZqnxOCJNdLiERAyDe-0caZ0WvCf7tED2KroJ-JbpJx9MvSqt0cu9G5PTsYruCl5AwTJWOoYB-kT5Mlfk6AM6xW4MMME_3KqFzP8Ey8aIz-qP0IAj-DySBerjBZ0E2kV3Gok8u-6Qer1FxA9oNYKEya9dQHJs_4CZN6q1Nt1Oqdeq3ZajSb9XajgneYtK6rjUaj3qp1atfvW7UP7ZcK_pdGrVWvO-2X_5yF0rQ)

### 플레이어 이동 플로우

플레이어 이동 및 위치 동기화:

[![](https://mermaid.ink/img/pako:eNqFU21v2jAQ_ivWfaJTikIghPhDpY2xF20I1K6VNkVCVnKDqInt2U4FQ_z3OTHtSAObP_jlzvc8z93Ze0hFhkBB468KeYrvc7ZWrEw4sYOlRigyLXLkxlkkUyZPc8m4IdMN4xyLO9Q6F7zrXxZsh6prnzP5mWvDLFvXuTAbVC5SO6-bnYjrm5s2KyXLL99Wy69vv89uV_PFw4z0th7ZeURLxOzqJY38iRk8q_gI3_JYGieBko9o3LZ3BHOn6zNKZON580_Yk9wp-cR4VuBcPGHPxXpECn1Be6dqbj4xn6i-s6qFzo0lberR1v733r3MauxytV3WxOVqZ9fL8C3175RgWcq0qROYbVOU5nIabi6EkOSDfVLI0g0RdauPVSM5JyWT7lqX-PRV1Nnx7HXfXaR1XFJ_pmG6SlO7d3cz_F-pO810b9LpWTz2Xim66uC2AcCDtcozoEZV6EGJqmT1EfZ1YAI25RIToHabMfWYQMIPNsb-kh9ClM9hSlTrDdCfrND2VDX9PP7hF6uy-lBNRcUN0OGkwQC6hy3QgR_0h_4kHgfjKB4N4iDwYAc0jPqjcTgMg2gSxePQj8ODB78bWr8_iUaxHVEcDvzY96PDH-LvZik?type=png)](https://mermaid.live/edit#pako:eNqFU21v2jAQ_ivWfaJTikIghPhDpY2xF20I1K6VNkVCVnKDqInt2U4FQ_z3OTHtSAObP_jlzvc8z93Ze0hFhkBB468KeYrvc7ZWrEw4sYOlRigyLXLkxlkkUyZPc8m4IdMN4xyLO9Q6F7zrXxZsh6prnzP5mWvDLFvXuTAbVC5SO6-bnYjrm5s2KyXLL99Wy69vv89uV_PFw4z0th7ZeURLxOzqJY38iRk8q_gI3_JYGieBko9o3LZ3BHOn6zNKZON580_Yk9wp-cR4VuBcPGHPxXpECn1Be6dqbj4xn6i-s6qFzo0lberR1v733r3MauxytV3WxOVqZ9fL8C3175RgWcq0qROYbVOU5nIabi6EkOSDfVLI0g0RdauPVSM5JyWT7lqX-PRV1Nnx7HXfXaR1XFJ_pmG6SlO7d3cz_F-pO810b9LpWTz2Xim66uC2AcCDtcozoEZV6EGJqmT1EfZ1YAI25RIToHabMfWYQMIPNsb-kh9ClM9hSlTrDdCfrND2VDX9PP7hF6uy-lBNRcUN0OGkwQC6hy3QgR_0h_4kHgfjKB4N4iDwYAc0jPqjcTgMg2gSxePQj8ODB78bWr8_iUaxHVEcDvzY96PDH-LvZik)

### 전투(공격) 플로우

플레이어와 몬스터 간 전투 시스템:

[![](https://mermaid.ink/img/pako:eNqVVGFv2jAQ_SuWPxUpRUCBEGtCYrCuW9uBBl82RUJufICFY2eOU7VD_Pc5iZuREbQtn2zfvffunc854EgxwASn8CMDGcGM062mcSiR_WhklEZTwUGa8iSh2vCIJ1QaNN1RKUEsIU25kufxhaCvoM_PH2nySaaGWrUGUhU_UbME_cybwss9F-Ji9FFZ3ibJudmBLutJQ1dqaet6PK77IGhxv1ovHibfPnxdT1aryfT-Ks1V15x5iHENkbFprYqmhrZ0pQxBH8GUy6vWXzKnVE6ModG-FHLpZfS6oUCjM7goX2sgQXdUMgGOPikoPVTz89vJKdIynfa6sDPXD4qy4rhqiSv2NPcMmqPQRqsYfV7OvyC-QRKAAWuAnhsowjPYXKryZJqKIlFcDgESPHVDe5LSIPAM-ZS_c7MzvqTzB8puk8zAHTcOmFbddQWkrs9vTf4XUiqiTFADhen3NIUZjekW6hdXXZhQKkG39oUCjXZoxyvvZfh_BG-5pMKJPeW68dZDNdXKV-syu2sFQXNZcrGqclbsHRakvXvs4a3mDJN8nD0cg45pvsWHPCnE9snGEGJil4zqfYhDebQY-56_KxW_wbTKtjtMNlSkdpclzJpx_7DqVFs90FOVSYNJf1BwYHLAL5iMRu2g3wv6o2Gv1-8Mbzz8ikm3E7T9wO8OR93gZjD0B8HRwz8L0U575PcD-_nBwKZ1Ov7xF0Try6s?type=png)](https://mermaid.live/edit#pako:eNqVVGFv2jAQ_SuWPxUpRUCBEGtCYrCuW9uBBl82RUJufICFY2eOU7VD_Pc5iZuREbQtn2zfvffunc854EgxwASn8CMDGcGM062mcSiR_WhklEZTwUGa8iSh2vCIJ1QaNN1RKUEsIU25kufxhaCvoM_PH2nySaaGWrUGUhU_UbME_cybwss9F-Ji9FFZ3ibJudmBLutJQ1dqaet6PK77IGhxv1ovHibfPnxdT1aryfT-Ks1V15x5iHENkbFprYqmhrZ0pQxBH8GUy6vWXzKnVE6ModG-FHLpZfS6oUCjM7goX2sgQXdUMgGOPikoPVTz89vJKdIynfa6sDPXD4qy4rhqiSv2NPcMmqPQRqsYfV7OvyC-QRKAAWuAnhsowjPYXKryZJqKIlFcDgESPHVDe5LSIPAM-ZS_c7MzvqTzB8puk8zAHTcOmFbddQWkrs9vTf4XUiqiTFADhen3NIUZjekW6hdXXZhQKkG39oUCjXZoxyvvZfh_BG-5pMKJPeW68dZDNdXKV-syu2sFQXNZcrGqclbsHRakvXvs4a3mDJN8nD0cg45pvsWHPCnE9snGEGJil4zqfYhDebQY-56_KxW_wbTKtjtMNlSkdpclzJpx_7DqVFs90FOVSYNJf1BwYHLAL5iMRu2g3wv6o2Gv1-8Mbzz8ikm3E7T9wO8OR93gZjD0B8HRwz8L0U575PcD-_nBwKZ1Ov7xF0Try6s)

### 아이템 사용 플로우

플레이어 아이템 사용 :

[![](https://mermaid.ink/img/pako:eNqVVttu4jAQ_RXLT7RKWwiX0qggdSndRlt2ERRpd4UUuckAFolDHQeVrfrv69wg10L7EJXxzJk5nmOP37HpWoA17MGrD8yEe0qWnDhzhuTfhnBBTbohTKCBTYGJon3mgS7AGRNzDSXLwdoU-JaaUFwc22QHvDxoRBhZli6yrSzE5bs5ixajyi76_UwpGhr_eDbGT3d_hhNjNh0a-vNwdPvCr_o1Kp0MainI98AwXZ-Jswgp-hJT0C0RUEYtYypJSbgHKMb_QsS-kHQd-cjUVmrokTDLhtiltgk3Ugkz6xGxQQCnIA6ebyf89swKXYm-KbPMF3VHQwPCkjx5_DxuuqHRN7Kk0VZgrvcAqI_qOcdU8zX0QJmV9KuMREYlKcNFKmNg1hkV90SQ88jTgmqUT-sWuw2gXg_N5blhnu-QFxvmOM8gUWgSFRBA8EY94SHKkCQTeByLevWl3KnYoX5vv1-ZmLwmBPfhakFsDwosi40htkh1Fi0ItSGWbF4KRdUOf-vPxsOd_jSbDA8xecFGB1NDU2DWT3ddA85dHncRZJXp_J5vmuB55QUcunBEiJViPPA-QWu6VQZXEMrXJHdMdlUVHlRxDyYHEt8vKHVdJO082NFtLzlXnyFOwHG3Md6Cu05WmWGbmFVWXpAtuLNgsQBTxNKuyJfsyZ1lPY5rq40hrySZHtB5WfeqQkfjmnMktKLWTw5LaWeqZVMuye8gBj7nktvZia6j01yDhZBjUZRV0RmGD9S2kwMzCcdANP5WGwU5GwWlx6AVjZ9gXjiEssxUPPE6mM4Gg-F0Wqn2zLzJkjgyI7_51LaCKgOjobOFi7ZSdi4_7eqplTwElAzc2WlAv9ZJ3_ZSS3HMvRawgpecWlgLhKZgB7jcWPkTvweRcyxW4MjRocl_LcLXwRD5kDHyifPXdZ0kjLv-coW18FJXsL-xZKb4iba3clkN8FApWGs0QwysveM3rDVb6uV1p9u5UduNpnp9rSp4J33Um0u12663Ou1mu9tSG80PBf8Ls9YvO61mp9vqdNtqs662242P_wQNQkA?type=png)](https://mermaid.live/edit#pako:eNqVVttu4jAQ_RXLT7RKWwiX0qggdSndRlt2ERRpd4UUuckAFolDHQeVrfrv69wg10L7EJXxzJk5nmOP37HpWoA17MGrD8yEe0qWnDhzhuTfhnBBTbohTKCBTYGJon3mgS7AGRNzDSXLwdoU-JaaUFwc22QHvDxoRBhZli6yrSzE5bs5ixajyi76_UwpGhr_eDbGT3d_hhNjNh0a-vNwdPvCr_o1Kp0MainI98AwXZ-Jswgp-hJT0C0RUEYtYypJSbgHKMb_QsS-kHQd-cjUVmrokTDLhtiltgk3Ugkz6xGxQQCnIA6ebyf89swKXYm-KbPMF3VHQwPCkjx5_DxuuqHRN7Kk0VZgrvcAqI_qOcdU8zX0QJmV9KuMREYlKcNFKmNg1hkV90SQ88jTgmqUT-sWuw2gXg_N5blhnu-QFxvmOM8gUWgSFRBA8EY94SHKkCQTeByLevWl3KnYoX5vv1-ZmLwmBPfhakFsDwosi40htkh1Fi0ItSGWbF4KRdUOf-vPxsOd_jSbDA8xecFGB1NDU2DWT3ddA85dHncRZJXp_J5vmuB55QUcunBEiJViPPA-QWu6VQZXEMrXJHdMdlUVHlRxDyYHEt8vKHVdJO082NFtLzlXnyFOwHG3Md6Cu05WmWGbmFVWXpAtuLNgsQBTxNKuyJfsyZ1lPY5rq40hrySZHtB5WfeqQkfjmnMktKLWTw5LaWeqZVMuye8gBj7nktvZia6j01yDhZBjUZRV0RmGD9S2kwMzCcdANP5WGwU5GwWlx6AVjZ9gXjiEssxUPPE6mM4Gg-F0Wqn2zLzJkjgyI7_51LaCKgOjobOFi7ZSdi4_7eqplTwElAzc2WlAv9ZJ3_ZSS3HMvRawgpecWlgLhKZgB7jcWPkTvweRcyxW4MjRocl_LcLXwRD5kDHyifPXdZ0kjLv-coW18FJXsL-xZKb4iba3clkN8FApWGs0QwysveM3rDVb6uV1p9u5UduNpnp9rSp4J33Um0u12663Ou1mu9tSG80PBf8Ls9YvO61mp9vqdNtqs662242P_wQNQkA)

---

## 🗂️ 클래스 다이어그램

전체 시스템의 클래스 구조 및 관계:

[![](https://mermaid.ink/img/pako:eNrNWntPG0kS_yojK4uMY6OEDSRBKBKxIVjitbC51Z0iWZ2ZBs8yr3T3ELxc_Nmv-jXTr7HNnXJa_sAzXdWvevyqunqeenGZ4N5BL84QpZMU3RGUfiki-Pvll2h8eX5-eRGdl0md4WgUfUQUR9OCYXKLYkwln-gZTa9QfI_ZKSqSDJPoSdL43-Fhqnt8-NA2vzx-xHHNcF92HJfA9MgGUcwetweS7Yf8CUxzgmJWksUm04wJRjBLDdTX-zMWsUWFtwdRXaTfajyrGFnaS192zi3ZzDlfqp4YJbDnufgxqA-Yr3LJp343Y8uoQousRMmaCZQkrHnGWYoLLhzxaxCsbRnt0BzdJkZDPEdkYK9AtP9Rkiy5wZSmZTGIvvO3GZWv5vxzVBQ4a_hi-R7gvMrQApMbTB7SGMOE4hX4xLvBd46qhilHlc0Bsum0wQliKLphpI5ZTWwb_AeOdy2x3cJmWfR4VVKvcdE0BhQB2yVgYbAPhixFjLgHiMZ89hUeDcoEk_QBJ4qYyDeDzjWSz-KazOZVsDn3mwnOUVrMkEF5-Qkzvoj-dqTXYhPVOoBurMhg-Vz1KSNpcRdRIPzOvaFTDjf3aZZN8K0lVN2b02apaUuCnY_oWuNpyqJ5aq5ignJ0h6NE_Bjt1_hbncK2wcppRIwXx7bjssyS8nsxy11KXs3ikrKAIZ0eXVwcn7WWNC4JjsZ8o7YZNcZOHmwk08pSxp_66s1SynAxu_UpuLIa66IkYCM4mYHxL4Fl6LjYEvoo7zJ2OJLudY4KEBsBFuVfuWwwGMG_Wi7uYEEW5YKKxfPSEbedlocbTIDpGicplQglbBbeDJ1Mi5T1uQyqkrDt6CsoztR3XfS3jffL4hrHGMy2LxHMpk1SGpcgpZgFyGD7lnTAA6z3gc3b7h4YDTRaBQqGggJ2YSnYMqKBUCZ_8lQ5aJRokFTg4Ji9FCKNHz7WtwZDAp6Bl058kSZTJL_VuMb-UuzICcuVT5aAPy4Ypn014ADck6FhRNO_MESYDBee-o6Lb3yyG5jVjrDD6KFMkybgDKOGGhrmJKvpXAzikW60VvtaXlJaQc03Ku_WoqSHvZrYLq2ALp-hOC7rgnVQC5RjHwjwAza2MZJBJ5_ZsahpXtjNdvyRjmc77rSAtkIETx48CPged-GQAaqQnfvB2kEhuaMhbGApdoBIAdGDYzq1FTJNhPullvtxRZU0ZTBDXwTiqqSOks64UEBHqZXCjFFxxBgYZ1-Hm4EMLp4twBCCRY7jxCF3WM4MYoPpLGludVrGOSAtc0xDtYmYls_sqDYSu8xntuqk-gMhPkePgdYuO8GPPm-qdD41zZCLiNPoUQao6eKuXj_OwWsYeKVquKnQ94JnUq7UPlcJz5bdZoBeEamTxguRUBkmQ86p4rinsAlkxN5Y15hi1g-oCwQJzVyq5jbkxixgCCmvdQlLgQ3siAg3nawMwWpvywaSz1LKfFhWMuRsuXwM800ZzjlTCr8OhxpCR6Z2pHM3Sis98u2lTOS-yXPUdszPRDL8ih1NJ8OoG0gvizOMVOzV_CZdHpHU4oVy1fNWpNZvcn8kgPwxouy8fMDHjzGuWGM-PE5x49FIMYyk5dMKYwtWwFrK7AELv4c0MmB_GjW2JBRA5BHi5wkn7U5tuXKstFYgGjQ2KL8yd1R6on5o0VnX00pTM46fstuytTove_L86ihJVMDzx9FqDYCn6mNp1w6Y8mgG6gqwdilrrWNuJhEzqC2lu5q4KvKMpXbnpThiQQQoF062oyQW8A1bFpdEFQYEtsLcybYJIqY8zsCMlZ33_6RlsRX9qY1s6eLpUrrnuiATEMnhIYVwlmFWFmb9IiQpG9QlDlnC4vlCGkc-yugQYstCb7q__cLts0qwxwWFEziXDpYJgcIAEYx969OwAfLxuMuK5w0oW7p728AVVeJOHavTx5anrtPT1mbHp62u85NCwys41qDsM5zIbVyVgbES1KmZnYTO2cYxy18vp17jiidXMnMn8GIZuEh0JPNWM71zytcNWxEm5JzerV6RVcZxzFQYl2Wj2t4WN9-ysTyggTqvwAREaOPNPrdxbBwEzo3crgwUinUSB7J8EfmwZ2aiqMGvbnh84VqohAnJ1SSyBv4lGvUe9cPCGiR0Yizzr2HFipjV1r9m1Hj3LExlx37kczLgpiFJiZ9kZ3HNPUrMzCtHMqFbH087RzpJwWHVKFxOvB42ye_a_GJFiG7TBm_4Mq9qhiHUKx4aWKKXiCnWwEymSOxug-XqulfY-DfAaHWO0ktZav0GENq1g_X4bPVw3EXQ2hDlKtINgG0X7yjV4HGziQ2AWNXHT8y6glU66azdh4v0m9Tom9WcXX6a6iqxOfVZeZcWnaW8UL1OWYlCJq49WXU31SdGXV9QeVbta3QU8xT5An-XU1u0K1JCiFNw2TeuA7pTDbFGVZZbU6xqahkdFY40vvevNZzq1K0VG8HATiahQ96Ut8qRreJBkVzeC1Gp6pGcQzI2NyfueLzbRWn2U2veJMRBtPrt7BmJ2Pk_gV_UWyDCWUo2sidjzC0L1WTVUt7CzEvKhpF8rikHNH07Q6l-Tr62CYRnOUK12msN4K--hxKvC1Bfy2xLv8tufqar_nF5fTYJuKq6inqGq-qM2LrE6qicKwxqi-K6jO8nf23BqCl9i-rgM0vfI7GsZ6OEUy3hIOEd6yVUBAhO9dwv43gVdIdFqu0GZ8DSyEHxNnbGpbEupzV1sjH-rK60UrEqiLJerfbnYBI7kstJE9N9XJFKBWuOSryFgOq_Qrf_Nyh6pm9VQ8P5ffLVFmczBi96abm0mt129uEkMe0K6jxHZNE3bC5Q5h2jeI4379N9rxOqUhjZnOKaFrflsgWPQKnGXt5RklwSebRQIzSmBCP5qlNuZzK2141GXmYuxxvgI6bNINuRwerpyVpQYvGuvQjjTKGL4dD1qKallVNx45DnXuCixxmPit6Vr7rmMGhdsPMzoxekvb8HghcIha2JXSfPzTL5mH-nJFN-AcGrumPUejbPDhuMyemdzFuwqPeeJNurzIj9_dLTpCMUmEkUn_B_B-sOkfxM2-W1N35Mj7YigZtOOuyC-wZZsVc-DISIteXsT-KjJ9WF38aLTNu6BMgwLNtgUsk4z2W6LczMzp6_FbM-ZduCtoN7vGgM_wFlNQ7tzGAOWNZpx3C3Kc6SNYOfwuhH7eldTGBELCu8mlYwLeaYpPKuDCSLuEDpPNUVZOcTu8N_7-wEzgTdvKHDfje3j9crR2YOq1eN4PUjVb0Lbc7-tmcwGtm3Nl1MbTl4BYdVv_M5jCJvF4thciGWEljsa33T5p3vU0ajD3b3IFcrgU5ytz7lf_VFhWJtb9otKl-Ncb3i0bq3Zd7sls5qTZpQgqzshYn8xs-niIUF7l7NFerUMDC9SfL213DpZJpPFrA3h8M1NqUZq5jM-W68orHNs7PzwdatR7XkZe7YYvIhvYvVsl-nNBdwRrM2xzVkVKxWMAQM0eRTS27KMF7hIbAQs_LA5zHPrmEO-9DQyWOdpcJcns8HF9Ppfc7pxYCINuH3DnWdarWz3DB-MnuLzMErl9wJ2GWeA8JMcMU_BSjiVN_gKVTga2y_BdEf59itBqsuVHeZuqb3hr07kia9A0ZqPOzlmOSIv_ZEgvClx-Y4x196B_CYIHL_pfel-AF9KlT8qyxz3Y2U9d28d3CLMgpvtTjfqY_nm1YiPnEY81Nv7-D13isxSO_gqffYO9jf23m9u_dq9937V_tv3u692R32FsC0_2rn3Zv913t779_tv919_f7XH8PeX2JaILx98x7-3v66u_tmd__V_o__ABbBGqE?type=png)](https://mermaid.live/edit#pako:eNrNWntPG0kS_yojK4uMY6OEDSRBKBKxIVjitbC51Z0iWZ2ZBs8yr3T3ELxc_Nmv-jXTr7HNnXJa_sAzXdWvevyqunqeenGZ4N5BL84QpZMU3RGUfiki-Pvll2h8eX5-eRGdl0md4WgUfUQUR9OCYXKLYkwln-gZTa9QfI_ZKSqSDJPoSdL43-Fhqnt8-NA2vzx-xHHNcF92HJfA9MgGUcwetweS7Yf8CUxzgmJWksUm04wJRjBLDdTX-zMWsUWFtwdRXaTfajyrGFnaS192zi3ZzDlfqp4YJbDnufgxqA-Yr3LJp343Y8uoQousRMmaCZQkrHnGWYoLLhzxaxCsbRnt0BzdJkZDPEdkYK9AtP9Rkiy5wZSmZTGIvvO3GZWv5vxzVBQ4a_hi-R7gvMrQApMbTB7SGMOE4hX4xLvBd46qhilHlc0Bsum0wQliKLphpI5ZTWwb_AeOdy2x3cJmWfR4VVKvcdE0BhQB2yVgYbAPhixFjLgHiMZ89hUeDcoEk_QBJ4qYyDeDzjWSz-KazOZVsDn3mwnOUVrMkEF5-Qkzvoj-dqTXYhPVOoBurMhg-Vz1KSNpcRdRIPzOvaFTDjf3aZZN8K0lVN2b02apaUuCnY_oWuNpyqJ5aq5ignJ0h6NE_Bjt1_hbncK2wcppRIwXx7bjssyS8nsxy11KXs3ikrKAIZ0eXVwcn7WWNC4JjsZ8o7YZNcZOHmwk08pSxp_66s1SynAxu_UpuLIa66IkYCM4mYHxL4Fl6LjYEvoo7zJ2OJLudY4KEBsBFuVfuWwwGMG_Wi7uYEEW5YKKxfPSEbedlocbTIDpGicplQglbBbeDJ1Mi5T1uQyqkrDt6CsoztR3XfS3jffL4hrHGMy2LxHMpk1SGpcgpZgFyGD7lnTAA6z3gc3b7h4YDTRaBQqGggJ2YSnYMqKBUCZ_8lQ5aJRokFTg4Ji9FCKNHz7WtwZDAp6Bl058kSZTJL_VuMb-UuzICcuVT5aAPy4Ypn014ADck6FhRNO_MESYDBee-o6Lb3yyG5jVjrDD6KFMkybgDKOGGhrmJKvpXAzikW60VvtaXlJaQc03Ku_WoqSHvZrYLq2ALp-hOC7rgnVQC5RjHwjwAza2MZJBJ5_ZsahpXtjNdvyRjmc77rSAtkIETx48CPged-GQAaqQnfvB2kEhuaMhbGApdoBIAdGDYzq1FTJNhPullvtxRZU0ZTBDXwTiqqSOks64UEBHqZXCjFFxxBgYZ1-Hm4EMLp4twBCCRY7jxCF3WM4MYoPpLGludVrGOSAtc0xDtYmYls_sqDYSu8xntuqk-gMhPkePgdYuO8GPPm-qdD41zZCLiNPoUQao6eKuXj_OwWsYeKVquKnQ94JnUq7UPlcJz5bdZoBeEamTxguRUBkmQ86p4rinsAlkxN5Y15hi1g-oCwQJzVyq5jbkxixgCCmvdQlLgQ3siAg3nawMwWpvywaSz1LKfFhWMuRsuXwM800ZzjlTCr8OhxpCR6Z2pHM3Sis98u2lTOS-yXPUdszPRDL8ih1NJ8OoG0gvizOMVOzV_CZdHpHU4oVy1fNWpNZvcn8kgPwxouy8fMDHjzGuWGM-PE5x49FIMYyk5dMKYwtWwFrK7AELv4c0MmB_GjW2JBRA5BHi5wkn7U5tuXKstFYgGjQ2KL8yd1R6on5o0VnX00pTM46fstuytTove_L86ihJVMDzx9FqDYCn6mNp1w6Y8mgG6gqwdilrrWNuJhEzqC2lu5q4KvKMpXbnpThiQQQoF062oyQW8A1bFpdEFQYEtsLcybYJIqY8zsCMlZ33_6RlsRX9qY1s6eLpUrrnuiATEMnhIYVwlmFWFmb9IiQpG9QlDlnC4vlCGkc-yugQYstCb7q__cLts0qwxwWFEziXDpYJgcIAEYx969OwAfLxuMuK5w0oW7p728AVVeJOHavTx5anrtPT1mbHp62u85NCwys41qDsM5zIbVyVgbES1KmZnYTO2cYxy18vp17jiidXMnMn8GIZuEh0JPNWM71zytcNWxEm5JzerV6RVcZxzFQYl2Wj2t4WN9-ysTyggTqvwAREaOPNPrdxbBwEzo3crgwUinUSB7J8EfmwZ2aiqMGvbnh84VqohAnJ1SSyBv4lGvUe9cPCGiR0Yizzr2HFipjV1r9m1Hj3LExlx37kczLgpiFJiZ9kZ3HNPUrMzCtHMqFbH087RzpJwWHVKFxOvB42ye_a_GJFiG7TBm_4Mq9qhiHUKx4aWKKXiCnWwEymSOxug-XqulfY-DfAaHWO0ktZav0GENq1g_X4bPVw3EXQ2hDlKtINgG0X7yjV4HGziQ2AWNXHT8y6glU66azdh4v0m9Tom9WcXX6a6iqxOfVZeZcWnaW8UL1OWYlCJq49WXU31SdGXV9QeVbta3QU8xT5An-XU1u0K1JCiFNw2TeuA7pTDbFGVZZbU6xqahkdFY40vvevNZzq1K0VG8HATiahQ96Ut8qRreJBkVzeC1Gp6pGcQzI2NyfueLzbRWn2U2veJMRBtPrt7BmJ2Pk_gV_UWyDCWUo2sidjzC0L1WTVUt7CzEvKhpF8rikHNH07Q6l-Tr62CYRnOUK12msN4K--hxKvC1Bfy2xLv8tufqar_nF5fTYJuKq6inqGq-qM2LrE6qicKwxqi-K6jO8nf23BqCl9i-rgM0vfI7GsZ6OEUy3hIOEd6yVUBAhO9dwv43gVdIdFqu0GZ8DSyEHxNnbGpbEupzV1sjH-rK60UrEqiLJerfbnYBI7kstJE9N9XJFKBWuOSryFgOq_Qrf_Nyh6pm9VQ8P5ffLVFmczBi96abm0mt129uEkMe0K6jxHZNE3bC5Q5h2jeI4379N9rxOqUhjZnOKaFrflsgWPQKnGXt5RklwSebRQIzSmBCP5qlNuZzK2141GXmYuxxvgI6bNINuRwerpyVpQYvGuvQjjTKGL4dD1qKallVNx45DnXuCixxmPit6Vr7rmMGhdsPMzoxekvb8HghcIha2JXSfPzTL5mH-nJFN-AcGrumPUejbPDhuMyemdzFuwqPeeJNurzIj9_dLTpCMUmEkUn_B_B-sOkfxM2-W1N35Mj7YigZtOOuyC-wZZsVc-DISIteXsT-KjJ9WF38aLTNu6BMgwLNtgUsk4z2W6LczMzp6_FbM-ZduCtoN7vGgM_wFlNQ7tzGAOWNZpx3C3Kc6SNYOfwuhH7eldTGBELCu8mlYwLeaYpPKuDCSLuEDpPNUVZOcTu8N_7-wEzgTdvKHDfje3j9crR2YOq1eN4PUjVb0Lbc7-tmcwGtm3Nl1MbTl4BYdVv_M5jCJvF4thciGWEljsa33T5p3vU0ajD3b3IFcrgU5ytz7lf_VFhWJtb9otKl-Ncb3i0bq3Zd7sls5qTZpQgqzshYn8xs-niIUF7l7NFerUMDC9SfL213DpZJpPFrA3h8M1NqUZq5jM-W68orHNs7PzwdatR7XkZe7YYvIhvYvVsl-nNBdwRrM2xzVkVKxWMAQM0eRTS27KMF7hIbAQs_LA5zHPrmEO-9DQyWOdpcJcns8HF9Ppfc7pxYCINuH3DnWdarWz3DB-MnuLzMErl9wJ2GWeA8JMcMU_BSjiVN_gKVTga2y_BdEf59itBqsuVHeZuqb3hr07kia9A0ZqPOzlmOSIv_ZEgvClx-Y4x196B_CYIHL_pfel-AF9KlT8qyxz3Y2U9d28d3CLMgpvtTjfqY_nm1YiPnEY81Nv7-D13isxSO_gqffYO9jf23m9u_dq9937V_tv3u692R32FsC0_2rn3Zv913t779_tv919_f7XH8PeX2JaILx98x7-3v66u_tmd__V_o__ABbBGqE)

> 전체 클래스 다이어그램은 각 서버의 핵심 컴포넌트와 그들 간의 관계를 보여줍니다.

---

## 📝 코딩 규칙

### 명명 규칙

프로젝트는 일관성과 가독성을 위해 엄격한 명명 규칙을 따릅니다.

| 요소 | 규칙 | 예시 |
|------|------|------|
| **클래스명** | UpperCamelCase | `Player`, `LoginManager` |
| **메서드명** | UpperCamelCase | `SendMessage()`, `GetStat()` |
| **멤버변수명** | `m_` + snake_case | `m_level`, `m_user_id`, `m_char_id` |
| **전역함수** | UpperCamelCase | `InitializeServer()` |
| **파일명** | 클래스명과 동일 | `Player.cpp`, `Player.h` |
| **상수** | 대문자 + `_` | `MAX_CLIENTS`, `BUFFER_SIZE` |
| **enum** | UpperCamelCase | `enum class PacketType` |

### 클래스 구조 예시

```cpp
class Player
{
public:
    // 생성자/소멸자
    Player();
    ~Player();

public:
    // Public 메서드 (Getter/Setter 먼저)
    void SetId(int char_id);
    int GetId() const;
    
    // 비즈니스 로직 메서드
    void ApplyStat();

private:
    // 멤버 변수 (m_ 접두사 사용)
    int m_char_id;
    std::string m_name;
    CharacterStat m_stat;
};
```

### 패킷 구조체 예시

```cpp
#pragma pack(push, 1)
struct PacketHeader
{
    uint16_t length;
    uint16_t type;
};
#pragma pack(pop)
```

### 설계 원칙

#### 도메인 모델
- 도메인 객체(Player, Monster 등)는 데이터뿐만 아니라 행위를 포함
- public 멤버 접근보다는 getter/setter 제공
- 메서드 내에서 상태 변경 캡슐화

```cpp
// ✅ 좋은 예: 캡슐화된 행위
void CharacterStat::IncreaseStr() {
    if (remainAp <= 0) return;
    base.str++;
    remainAp--;
}

// ❌ 나쁜 예: 직접 멤버 접근
stat.str++;  // 이렇게 하지 마세요
```

#### 핸들러 패턴
- 핸들러는 얇게 유지하고 서비스에 위임
- 핸들러에 비즈니스 로직 금지
- 핸들러에서 직접 데이터베이스 접근 금지

```cpp
// ✅ 좋은 예: 얇은 핸들러
void PlayerHandler::HandleStatView(Player& player)
{
    const CharacterStat& stat = player.GetStat();
    StatInfoPacket pkt = StatPacketFactory::MakeStatInfo(stat);
    Send(pkt);
}
```

#### 계층별 규칙

| 계층 | 규칙 |
|------|------|
| **Database** | 이 계층 외부에서 SQL 쿼리 금지 |
| **Packet** | 핸들러는 얇게 유지하고 서비스에 위임 |
| **IPC** | 클라이언트 대면 패킷 계층과 분리 |
| **Repository** | 원시 데이터가 아닌 도메인 객체 반환 |

### 핵심 아키텍처 원칙

1. **관심사의 분리**: 네트워크, 비즈니스 로직, 데이터 접근이 엄격히 분리
2. **도메인 주도 설계**: 핵심 도메인 모델은 데이터뿐만 아니라 행위를 포함
3. **의존성 방향**: Handlers → Services → Repositories → Database
4. **IPC 격리**: 서버 간 통신은 클라이언트 대면 코드와 분리
5. **얇은 핸들러**: 패킷 핸들러는 서비스 계층에 위임하며 비즈니스 로직을 포함하지 않음

### 코드 품질

- **에러 처리**: 시스템 콜의 반환 값 확인, 적절한 에러 코드 및 로깅
- **메모리 관리**: RAII 및 스마트 포인터 선호, 커넥션 풀링 사용
- **스레딩**: 커스텀 스레드 풀 사용, 공유 메모리보다 메시지 전달 선호
- **로깅**: slog 라이브러리 사용, 적절한 컨텍스트 포함 (서버 타입, 플레이어 ID 등)

---

## 📄 라이선스

이 프로젝트는 MIT 라이선스 하에 배포됩니다. 자세한 내용은 [LICENSE](LICENSE) 파일을 참조하세요.

---

## 👥 기여

이 프로젝트는 포트폴리오 목적으로 개발되었습니다.

---

## 📞 연락처

프로젝트에 대한 문의사항이 있으시면 이슈를 등록해주세요.

---

<div align="center">

**LL2Games RPG Server** - C++17 기반 고성능 MMORPG 게임 서버

Made with ❤️ using C++17

</div>
