#include "ChannelCommandSender.h"
#include "K_slog.h"

void ChannelCommandSender::Send(const std::string &payload)
{
    int msqid;
    struct msgbuf_ msg_send, msg_recv;

    memset(&msg_send, 0x00, sizeof(struct msgbuf_));
    memset(&msg_recv, 0x00, sizeof(struct msgbuf_));

    // 메시지 큐 생성
    if ((msqid = msgget(MSGKEY, IPC_CREAT | 0666)) < 0)
    {
        perror("msgget");
        exit(1);
    }

    // 메시지 전송 -> ChannelD
    msg_send.mtype = MSG_TYPE_SEND;
    strcpy(msg_send.mtext, payload.c_str());
    msgsnd(msqid, &msg_send, sizeof(msg_send.mtext), 0);

#if 0
    // 메시지 수신 <- ChannelD
    msgrcv(msqid, &msg_recv, sizeof(msg_recv.mtext), MSG_TYPE_RECV, 0);
    printf("받은 메시지[%d]: %s\n", msg_recv.mtype, msg_recv.mtext);
#endif

}