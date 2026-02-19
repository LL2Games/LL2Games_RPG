#pragma once
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


struct msgbuf_
{
    long mtype;
    char mtext[1024];
};

class CommandMsgQueue
{
public:
    CommandMsgQueue(const int msgKey, const int typeSend, const int typeRecv);
    ~CommandMsgQueue();
    int Send(const std::string& payload);
    int Recv(std::string& payload);

private:
    int m_msqId;
    int m_msgKey;
    int m_typeSend;
    int m_typeRecv;
};
