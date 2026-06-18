#pragma once

#include "Task.h"
#include "MapInstance.h"

class MapUpdateTask : public Task
{
public:
    MapUpdateTask(MapInstance* map) : m_map(map) {}
    void Execute() override
    {
        if (m_map)
            m_map->Update(); //여기서 몬스터 이동처리
    }
private:
    MapInstance* m_map;
};