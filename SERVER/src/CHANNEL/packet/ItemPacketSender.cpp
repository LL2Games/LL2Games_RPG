#include "ItemPacketSender.h"  
#include "ChannelSession.h"

void ItemPacketSender::SendSpawnItem(std::vector<DropSpawnInfo> spawnedInfos, std::unordered_map<int, Player*>& playerList)
{

    std::vector<std::string> payload;	
	payload.push_back(std::to_string(spawnedInfos.size()));

    for (auto& [id, p] : playerList)
    {
    	if (!p) continue;
        auto session = p->GetSession();
        if (!session) continue;
		for(size_t i =0; i < spawnedInfos.size(); i++)
		{
        	payload.push_back(std::to_string(spawnedInfos[i].dropId));
        	payload.push_back(std::to_string(spawnedInfos[i].itemId));
        	payload.push_back(std::to_string(spawnedInfos[i].count));
        	payload.push_back(std::to_string(spawnedInfos[i].xPos));
        	payload.push_back(std::to_string(spawnedInfos[i].yPos));
		}
		session->Send(PKT_DROPITEMS, payload);
    }	
}

void ItemPacketSender::SendAddItem(Player *player, std::vector<AddItemResult> &addItemResult)
{
	if (player == nullptr)
        return;

    auto session = player->GetSession();
    if (session == nullptr)
        return;

    if (addItemResult.empty())
	{
		return;
	}
        
	std::vector<std::string> payload;	
	payload.push_back(std::to_string(addItemResult.size()));
	
	for(size_t i =0; i < addItemResult.size(); i++)
	{
		K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] inventoryType [%d]\n", __FILE__, __FUNCTION__, __LINE__,addItemResult[i].inventoryType);
		K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] slotPos [%d]\n", __FILE__, __FUNCTION__, __LINE__,addItemResult[i].slotPos);
		K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] itemId [%d]\n", __FILE__, __FUNCTION__, __LINE__,addItemResult[i].itemId);
		K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] itemCount [%d]\n", __FILE__, __FUNCTION__, __LINE__,addItemResult[i].itemCount);

		payload.push_back(std::to_string(addItemResult[i].inventoryType));
    	payload.push_back(std::to_string(addItemResult[i].slotPos));
    	payload.push_back(std::to_string(addItemResult[i].itemId));
    	payload.push_back(std::to_string(addItemResult[i].itemCount));  
	}
	session->Send(PKT_PLAYER_PICKUP_ITEM, payload);
	K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Send PickUpItem Success\n", __FILE__, __FUNCTION__, __LINE__);
}

void ItemPacketSender::SendRemoveDropItem(const std::vector<int>& removeItems, const std::unordered_map<int, Player*>& playerList)
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
		}

		session->Send(PKT_REMOVEITEMS, payload);
		K_slog_trace(K_SLOG_TRACE, "[%s : %s : %d] Send PKT_REMOVEITEMS Success\n", __FILE__, __FUNCTION__, __LINE__);
    }	
}

