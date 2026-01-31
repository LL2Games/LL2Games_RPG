#pragma once

#include <thread>
#include <atomic>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>

#include "CommandMsgQueue.h"

#define MSG_KEY 1234
#define MSG_COMMAND_SEND 1
#define MSG_COMMAND_RECV 0

class CommandReceiver {
public:
    CommandReceiver();
    ~CommandReceiver();

    void Start();
    void Stop();

private:
    void Run();

private:
    CommandMsgQueue m_mq;
    int m_msgid{-1};
    std::thread m_thread;
    std::atomic<bool> m_running{false};
};
