#pragma once

#include "Task.h"
#include "ChannelServer.h"

class ChannelStateUpdateTask : public Task
{
public:
    ChannelStateUpdateTask(ChannelServer* server, const int ttl) : m_server(server), m_ttl(ttl) {}
    void Execute() override
    {
        if (m_server)
            m_server->UpdateChannelStateToRedis(m_ttl);
    }
private:
    ChannelServer* m_server;
    const int m_ttl;
};