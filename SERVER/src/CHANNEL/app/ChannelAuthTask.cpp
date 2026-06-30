#include "ChannelAuthTask.h"
#include "ChannelServer.h"
#include "ChannelAuthResult.h"
#include "PacketParser.h"
#include "K_slog.h"
#include <cstring>
#include <stdexcept>

namespace
{
int ReadCharacterIdFromPayload(const std::string& payload)
{
    if (payload.size() < sizeof(uint16_t))
    {
        throw std::runtime_error("invalid auth payload");
    }

    uint16_t len = 0;
    std::memcpy(&len, payload.data(), sizeof(uint16_t));

    if (payload.size() < sizeof(uint16_t) + len)
    {
        throw std::runtime_error("invalid auth payload length");
    }

    std::string value(payload.data() + sizeof(uint16_t), len);
    return std::stoi(value);
}
}

ChannelAuthTask::ChannelAuthTask(ChannelServer* server, int fd, std::string payload)
    : m_server(server),
      m_fd(fd),
      m_payload(std::move(payload))
{
}

void ChannelAuthTask::Execute()
{
    ChannelAuthResult result;
    result.fd = m_fd;
    result.success = false;

    try
    {
        // 기존 PKT_CHANNEL_AUTH payload 파싱 방식에 맞춰야 함.
        // 지금 테스트 payload가 characterId 하나라면 이 형태.
        int characterId = ReadCharacterIdFromPayload(m_payload);

        result.characterId = characterId;

        std::unique_ptr<Player> player;

        {
            std::lock_guard<std::mutex> lock(m_server->GetAuthLoadMutex());
            player = m_server->GetPlayerService()->LoadPlayer(characterId);
        }

        if (!player)
        {
            result.error = "player load failed";
            m_server->PushAuthResult(std::move(result));
            return;
        }

        // 여기서 DB/Redis 로드만 수행.
        // session->Send(), PlayerManager AddPlayer는 절대 하지 않음.
        // 인벤토리/스킬 로드가 player에 데이터를 채우는 구조라면 여기서 호출 가능.

        result.success = true;
        result.player = std::move(player);

        m_server->PushAuthResult(std::move(result));
    }
    catch (const std::exception& e)
    {
        result.error = e.what();
        m_server->PushAuthResult(std::move(result));
    }
}