참고:https://github.com/eliotjang/tower-defense-multiplayer-game-project?tab=readme-ov-file

# LoginD
### 로그인 (0x01)
- Packet ID   : 0x01
- Description : 채널 채팅 메시지 전송

+ Direction   : Client → Server

| 필드 명 | 타입 | 설명 |
| --- | --- | --- |
| account_id    | string | 아이디 |
| password    | string | 비밀번호 |


| 필드 명 | 타입 | 설명 |
| --- | --- | --- |
| account_id    | string | 아이디 |
| password    | string | 비밀번호 |

<br>
<br>

# ChatD
###  (0x01)
- Packet ID   : 0x01
- Direction   : Client → Server
- Description : 채널 채팅 메시지 전송
------------------------------------------------
| 필드 명 | 타입 | 설명 |
| --- | --- | --- |
| account_id    | string | 아이디 |
| password    | string | 비밀번호 |

[CHAT_SEND]
Packet ID   : 0x0201
Direction   : C → S
Description : 채널 채팅 전송

Header
------------------------------------------------
uint16 packetSize
uint16 packetType (0x0201)

Body
------------------------------------------------
| Type | Field |
| ---------- | ------ |
|uint32 | channelId |
|uint16 | messageLength |
|char |  message[messageLength]|


| 필드 명 | 타입 | 설명 |
| --- | --- | --- |
| status | string | “success” |
| message | string | “몬스터 처치 성공.” |
| score | int | 처치 후 유저 점수 |