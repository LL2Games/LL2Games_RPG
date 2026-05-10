#include "ItemPacketSender.h"  
#include "ChannelSession.h"

void ItemPacketSender::SendSpawnItem(std::vector<DropSpawnInfo> spawnedInfos, std::unordered_map<int, Player*>& playerList)
{
    for (auto& [id, p] : playerList)
    {
        if (!p) continue;
        auto session = p->GetSession();
        if (!session) continue;
        std::vector<std::string> payload;
		
		for(size_t i =0; i < spawnedInfos.size(); i++)
		{
        	payload.push_back(std::to_string(spawnedInfos[i].dropId));
        	payload.push_back(std::to_string(spawnedInfos[i].itemId));
        	payload.push_back(std::to_string(static_cast<int>(spawnedInfos[i].type)));
        	payload.push_back(std::to_string(spawnedInfos[i].count));
        	payload.push_back(std::to_string(spawnedInfos[i].xPos));
        	payload.push_back(std::to_string(spawnedInfos[i].yPos));
        	session->Send(PKT_DROPITEMS, payload);
		}
    }	
}


void ItemPacketSender::SendRemoveItem(std::vector<int> removeItems, std::unordered_map<int, Player*>& playerList)
{
    for (auto& [id, p] : playerList)
    {
        if (!p) continue;
        auto session = p->GetSession();
        if (!session) continue;
        std::vector<std::string> payload;
		
		for(size_t i =0; i < removeItems.size(); i++)
		{
        	payload.push_back(std::to_string(removeItems[i]));

        	session->Send(PKT_REMOVEITEMS, payload);
		}
    }	
}
