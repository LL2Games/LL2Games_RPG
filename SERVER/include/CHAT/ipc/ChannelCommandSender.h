#pragma once
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGKEY 1234
#define MSG_TYPE_SEND 0
#define MSG_TYPE_RECV 1

struct msgbuf_
{
    long mtype;
    char mtext[1024];
};

class ChannelCommandSender
{
public:
    void Send(const std::string& payload);
};
