# 🎯 **최종 추천 폴더 구조**

```
SERVER/
├─ include/
├─ src/
│    ├─ core/
│    ├─ game/
│    ├─ net/
│    ├─ db/
│    └─ util/
├─ conf/
├─ scripts/
├─ build/
├─ bin/
├─ lib/
├─ logs/
├─ tools/
├─ docs/
└─ test/
```

---


# 🎯 **직접 구현할 핵심 디자인 패턴 7개**

✔ Singleton → Config, LogManager, DBPool
✔ Factory → PacketFactory, CommandFactory
✔ Strategy → Skill 처리, AI 행동, 패킷 핸들러
✔ Observer → Chat/Lobby 브로드캐스트
✔ State → Player 상태 (LOGGED_IN, IN_WORLD 등)
✔ Command → 클라이언트 패킷 처리 로직
✔ Reactor / Proactor → 이벤트 기반 IO (select/epoll)

# **📡 7️⃣ 통신 프로토콜 요약**

| **통신** | **용도** | **포트** | **패킷 예시** |
| --- | --- | --- | --- |
| TCP(Login) | 인증, 캐릭터선택 | 5000 | PK_LOGIN_REQ, PK_LOGIN_ACK |
| TCP(Chat) | 채팅/파티/길드 | 5200 | PK_CHAT_MSG, PK_PARTY_MSG |
| TCP(Game) | 명령, 맵 전환 | 5301 | PK_ENTER_MAP, PK_MOVE_ZONE |
| UDP(Game) | 실시간 위치/전투 | 5401 | PK_GAME_MOVE, PK_ATTACK |