#include "CommandReceiver.h"

CommandReceiver::CommandReceiver(): m_mq(MSG_KEY, MSG_COMMAND_SEND, MSG_COMMAND_RECV)
{
}

CommandReceiver::~CommandReceiver() {
    Stop();
}

void CommandReceiver::Start() {
    m_running = true;
    m_thread = std::thread(&CommandReceiver::Run, this);
}

void CommandReceiver::Stop() {
    m_running = false;

    // msgrcv는 blocking → dummy wake-up
    // MQMessage dummy{};
    // dummy.mtype = MSG_TYPE_CHAT_TO_CHANNEL;
    // msgsnd(m_msgid, &dummy, sizeof(dummy.payload), 0);

    if (m_thread.joinable())
        m_thread.join();
}

void CommandReceiver::Run() {
    while (m_running) {
        // MQMessage msg{};
        // ssize_t ret = msgrcv(
        //     m_msgid,
        //     &msg,
        //     sizeof(msg.payload),
        //     MSG_TYPE_CHAT_TO_CHANNEL,
        //     0   // blocking
        // );
        std::string msg;
        ssize_t ret = m_mq.Recv(msg);

        if (ret <= 0)
            continue;

        std::cout << "[ChannelD] MQ recv: " << msg.c_str() << std::endl;

        // TODO:
        // 1. Command 변환
        // 2. ThreadPool::Submit(...)
    }
}
