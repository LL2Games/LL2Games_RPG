#pragma once
#include "common.h"
#include "PlayerManager.h"
#include "CommonEnum.h"
#include "Skill_Info.h"
#include "Monster.h"
#include "MonsterManager.h"
#include "Item.h"
#include "Player.h"
#include "ChannelSession.h"
#include "Skill_Info.h"

#include <nlohmann/json.hpp>
#include <functional>



class MapInstance 
{
public:
    MapInstance();
    ~MapInstance();
    
    int Init(const MapInitData& data);
    int InitSpawnMonster();
    int SetMonster(MonsterType monsterType);
    //int LoadMonsters(int monster_id);
    int Update();
    int SpawnMonster();

    void RemoveMap();
	
	int checkPlayer(int PlayerID);
	
	// 플레이어 들어왔을 때 처리 함수
	void OnEnter(int PlayerID, Player* player);

	// 플레이어 나갔을 때 처리 함수
	void OnLeave(int PlayerID);

    // 몬스터 죽었을 떄 관리
    void HandleMonsterDead(Monster& monster);

    void GiveExp(int platerID, float exp);

    void GiveItem(int ItemGroup);

    void HandleMove(Player* sender, Vec2 pos, float speed);

    void BroadcastMoveExcept(Player* sender, Vec2 pos, float speed);

    void BroadcastMonsterHit(Player* Attacker, std::string SkillID, std::vector<MonsterHitResult> result);

    void ResolveSkillHit(Player* Attacker, SkillDef& skillDef, std::vector<std::pair<Monster*, int>> hits);

public:

    // int 매개변수를 받는 콜백 함수 이름 지정
    using DestroyReqFn = std::function<void(int m_mapID)>;

    // 나중에 호출할 콜백 함수를 변수에 저장하는 함수
    void SetDestroyCallback(DestroyReqFn cb) {m_onDestroyReq = std::move(cb);}

    uint16_t GetMapId() {return m_mapID;}


    std::vector<Monster>& GetMonsterList(){ return m_monsterList;};

private:
   	// 플레이어가 맵에 있는지 없는지 판단 변수
    bool m_has_player;
    bool m_destroyRequested;
	
	// 플레이어 수 
	uint16_t m_playerCount;
    uint16_t m_mapID;

    std::unordered_map<int, Player*> m_playerList;
   
    std::vector<MonsterSpawnData> m_monsterSpawnList;
    std::vector<MonsterSpawnData>::iterator m_monsterSpawnListIter;

    std::vector<Monster> m_monsterList;
    std::vector<Monster>::iterator m_monsterListIter;

    std::vector<Item> m_itemList;
	
	MonsterManager* m_monsterManager;
	
	// Map에 플레이어가 없을 때 딱 시간 찍는 변수
	std::chrono::steady_clock::time_point m_emptyTime;
    // Map 사라지는 제한 시간
    std::chrono::minutes m_limit;

    DestroyReqFn m_onDestroyReq;


};