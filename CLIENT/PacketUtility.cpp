#include "pch.h"
#include "PacketUtility.h"
#include "MySocket.h"

void PacketUtility::SendPacket(CMySocket* sock, uint16_t type, const std::string& data)
{
	PacketHeader hdr;
	hdr.type = type;
	hdr.length = sizeof(PacketHeader) + data.size();

	std::string packet;
	packet.append((char*)&hdr, sizeof(hdr));
	packet.append(data);

	sock->Send(packet.data(), packet.size());
}