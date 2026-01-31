#include "CommandMsgQueue.h"
#include "K_slog.h"

CommandMsgQueue::CommandMsgQueue(const int msgKey, const int typeSend, const int typeRecv) : m_msgKey(msgKey), m_typeSend(typeSend), m_typeRecv(typeRecv)
{
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] CommandMsgQueue Constructor msgKey[%d], typeSend[%d], typeRecv[%d]", __FILE__, __LINE__, msgKey, typeSend, typeRecv); 
    if ((m_msqId = msgget(m_msgKey, IPC_CREAT | 0666)) < 0)
    {
        perror("msgget");
    }
}

CommandMsgQueue::~CommandMsgQueue()
{
    if (m_msqId >= 0)
    {
        if (msgctl(m_msqId, IPC_RMID, nullptr) < 0)
        {
            perror("msgctl IPC_RMID");
        }
        else
        {
            K_slog_trace(K_SLOG_DEBUG, "[%s][%d]Message queue removed", __FILE__, __LINE__);
        }

        m_msqId = -1;
    }
}

int CommandMsgQueue::Send(const std::string& payload)
{
    struct msgbuf_ msg_send;

    memset(&msg_send, 0x00, sizeof(struct msgbuf_));

    // 메시지 전송
    msg_send.mtype = m_typeSend;
    strcpy(msg_send.mtext, payload.c_str());
    msgsnd(m_msqId, &msg_send, sizeof(msg_send.mtext), 0);

    return 0;
}
    
int CommandMsgQueue::Recv(std::string& payload)
{
    struct msgbuf_ msg_recv;

    memset(&msg_recv, 0x00, sizeof(struct msgbuf_));

    // 메시지 수신
    msgrcv(m_msqId, &msg_recv, sizeof(msg_recv.mtext), m_typeRecv, 0);
    payload = std::string(msg_recv.mtext);
    K_slog_trace(K_SLOG_DEBUG, "[%s][%d] 받은 메시지[%d]: %s", __FILE__, __LINE__, msg_recv.mtype, msg_recv.mtext); 

    return 0;
}

