// 클라이언트 측 코드
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include "Packet.h"

#define MSGKEY 1234
#define MSG_COMMAND_SEND 0
#define MSG_COMMAND_RECV 1

struct msgbuf_Info
{
    std::string player_id;
};

struct msgbuf_
{
    long mtype;
    char mtext[1024];
};

int MsgQueueUtil(const std::string &msg, PacketContext *ctx)
{
    if (msg.rfind("/찾기 ") != 0)
    {
        // not command
        return 0;
    }

    int msqid;
    struct msgbuf_ msg_buf;

    // 메시지 큐 생성
    if ((msqid = msgget(MSGKEY, IPC_CREAT | 0666)) < 0)
    {
        perror("msgget");
        exit(1);
    }

    // 메시지 전송
    msg_buf.mtype = MSG_COMMAND_SEND;
    strcpy(msg_buf.mtext, input);
    msgsnd(msqid, &msg_buf, sizeof(msg_buf.mtext), 0);

    printf("recv?");
    memset(input, 0x00, sizeof(input));
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0; // 개행 제거
    // 메시지 수신
    memset(&msg2, 0x00, sizeof(msg2));
    msgrcv(msqid, &msg2, sizeof(msg2.mtext), 1, 0);
    printf("받은 메시지[%d]: %s\n", msg2.mtype, msg2.mtext);

    return 0;
}