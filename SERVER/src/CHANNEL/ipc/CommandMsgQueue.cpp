#include "CommandMsgQueue.h"


 CommandMsgQueue::CommandMsgQueue(const int msgKey, const int typeSend, const int typeRecv)
 {
    m_msgKey = msgKey;
    m_typeSend = typeSend;
    m_typeRecv = typeRecv;
 }
 
 CommandMsgQueue::~CommandMsgQueue()
 {

 }
int CommandMsgQueue::Send(const std::string& payload)
{
    (void)payload;
    return 0;
}
int CommandMsgQueue::Recv(std::string& payload)
{
    (void)payload;
    return 0;
}
