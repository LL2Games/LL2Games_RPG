#include "COMMON/common.h"
#include "CHANNEL/core/ChannelServer.h"


#define PORT 9999

int main()
{
    
    bool Start = false;
    ChannelServer channelServer;

    Start = channelServer.Init(PORT);

    if(Start == false)
    {
        return -1;
    }

}