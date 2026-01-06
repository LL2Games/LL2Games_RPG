#include "CHANNEL/core/common.h"
#include "CHANNEL/core/MapInstance.h"



MapInstance::MapInstance()
{

}

MapInstance::~MapInstance()
{

}

int MapInstance::Init(const MapInitData& data)
{
    // 몬스터 상태 관리 
    for (const auto& info : data.spawns) {
        for (int i = 0; i < info.spawnNum; ++i) {
            m_spawnList.emplace_back(info);
        }
    }
        return 0;
}

int MapInstance::Update(float deltaTime)
{
    
}

int MapInstance::SpawnMonster()
{

}

int MapInstance::RemoveMonster()
{
    
}