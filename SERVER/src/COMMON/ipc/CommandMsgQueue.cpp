#include "CommandMsgQueue.h"
#include "K_slog.h"

CommandMsgQueue::CommandMsgQueue(const int msgKey, const int typeSend, const int typeRecv) : m_msgKey(msgKey), m_typeSend(typeSend), m_typeRecv(typeRecv)
{}

int CommandMsgQueue::Send(const std::string& payload)
{
    int msqid;
    struct msgbuf_ msg_send;

    memset(&msg_send, 0x00, sizeof(struct msgbuf_));

    // 메시지 큐 생성
    if ((msqid = msgget(m_msgKey, IPC_CREAT | 0666)) < 0)
    {
        perror("msgget");
        return -1;
    }

    // 메시지 전송
    msg_send.mtype = m_typeSend;
    strcpy(msg_send.mtext, payload.c_str());
    msgsnd(msqid, &msg_send, sizeof(msg_send.mtext), 0);

    return 0;
}
    
int CommandMsgQueue::Recv(std::string& payload)
{
	int msqid;
    struct msgbuf_ msg_recv;

    memset(&msg_recv, 0x00, sizeof(struct msgbuf_));

    // 메시지 큐 생성
    if ((msqid = msgget(m_msgKey, IPC_CREAT | 0666)) < 0)
    {
        perror("msgget");
        return -1;
    }


    // 메시지 수신
    msgrcv(m_msgKey, &msg_recv, sizeof(msg_recv.mtext), m_typeRecv, 0);
    payload = std::string(msg_recv.mtext);
    //printf("받은 메시지[%d]: %s\n", msg_recv.mtype, msg_recv.mtext);

    return 0;
}

