- packet 형식
    
    ```cpp
    struct PacketHeader
    {
        uint16_t length; // 전체 패킷 길이
        uint16_t type;   // 패킷 타입
    };
    
    struct ParsedPacket
    {
        uint16_t type;
        std::string payload;
    };
    [LV][LV][LV]
    
    // 패킷타입 정의
    enum PACKET_TYPE : uint16_t {
        PKT_LOGIN = 0x01,
        PKT_REGISTER = 0x02,
        PKT_CHAT_INIT,
        PKT_CHAT,
        PKT_INIT_WORLD,
        PKT_SELECT_CHARACTER,
        PKT_SELECT_CHANNEL,
        PKT_INIT_CHANNEL,
        PKT_CHANNEL_AUTH
    };
    
    ```
    

## LoginD

### 로그인 (0x01)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | account_id | 아이디 | “gunoo22” |
| string | password | 비밀번호 | “q1w2e3r4t5!@” |

> 응답(현재 닉네임 리턴받게 되어있음 수정해야함 260208)
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |
| string | account_id | 아이디 | “gunoo22” |

---

## ChatD

---

### 초기화(0x03)

> 요청 (현재 account_id로 되어있음 char_id로 수정해야함 260208)
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | char_id | 아이디 | “1” |

> 응답 (현재 account_id도 리턴받게 되어있음 수정해야함 260208)
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |
| string | nickname | 닉네임 | “타락파워전사” |

### 채팅(0x04)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | message | 메시지 | “하이요~” |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |
| string | message | 메시지 | “하이요~” |

## WorldD

---

### 초기화(0x05)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | account_id | 아이디 | “gunoo22” |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |

### 채널선택 창(N)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | world_id | 월드 아이디 | “zenis” |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |
| int | ratio_1 | 1채널 비율 최대 10을 기준으로 계산 | 7 |
| int | ratio_2 | 2채널 비율 최대 10을 기준으로 계산 | 4 |

### 채널선택

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | channel_id | 채널 아이디 | “ch_01” |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |
| string | ip | ip | “100.12.3.5” |
| string | port | port | “2000” |

### 캐릭터선택 창(N)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
|  |  |  |  |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |
| string | charid$nick$level$job | charid$nick$level$job | “01$타락파워전사$140$1” |

### 캐릭터선택

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | char_id | 캐릭터 아이디 | “1” |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |

## ChannelD

---

### 초기화(0x09)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | char_id | 캐릭터 ID | “1” |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |

### 맵 초기화(0x0A)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | char_id | 캐릭터 ID | “1” |
| string | map_id | 맵 ID | “00000001” |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |

### 플레이어 이동(0x0B)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | xPos | x 좌표 |  |
| string | yPos | y 좌표 |  |
| string | speed | 이동속도 |  |

> 응답 (본인 제외, 동일 맵 내 전체 플레이어에게 브로드캐스트)
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status |  |
| string | xPos | x 좌표 |  |
| string | yPos | y 좌표 |  |
| string | speed | 이동속도 |  |

### 플레이어 공격(0x0C)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |

### 능력치 조회(0x1000)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
|  |  |  |  |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |
| int | str | 힘 | 42 |
| int | dex | 민첩 | 20 |
| int | intel | 지능 | 4 |
| int | luk | 운 | 4 |
| int | max_hp | 최대HP | 1020 |
| int | max_mp | 최대MP | 120 |
| int | cur_hp | 현재HP | 1020 |
| int | cur_mp | 현재MP | 20 |
| int | level | 레벨 | 24 |
| int | remain_ap | 남은 능력치 | 0 |

### 능력치 분배(0x1001)

> 요청
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | statType | 스탯타입 | “str” |

> 응답
> 

| 타입 | 필드 명 | 설명 | 예시 |
| --- | --- | --- | --- |
| string | ok | status | “ok” |