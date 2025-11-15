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