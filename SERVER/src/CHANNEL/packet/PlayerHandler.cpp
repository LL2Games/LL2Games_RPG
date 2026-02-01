#include "PlayerHandler.h"


PlayerHandler::PlayerHandler(uint16_t type) : m_type(type)
{

}


void PlayerHandler::Execute(PacketContext * ctx)
{
    (void)ctx;
}

void PlayerHandler::Move(PacketContext * ctx)
{
     (void)ctx;
}

void PlayerHandler::Attack(PacketContext * ctx)
{   
     (void)ctx;
}

void PlayerHandler::OnDamaged(PacketContext * ctx)
{
     (void)ctx;
}

void PlayerHandler::UseItem(PacketContext * ctx)
{
     (void)ctx;
}
