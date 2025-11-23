#pragma once

class IPacketHandler
{
public:
	virtual void Execute() = 0;
};