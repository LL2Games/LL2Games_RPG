#pragma once

#include <string>
#include "Packet.h"

class CMySocket;

class PacketUtility
{
public:
	static void SendPacket(CMySocket* sock, uint16_t type, const std::string& data);
};

