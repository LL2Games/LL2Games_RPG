#pragma once
#include "common.h"
#include "CommonEnum.h"
#include "Item.h"
#include <nlohmann/json.hpp>
#include <filesystem> //C++ 17 g++ 사용의 경우 8버전 이상이 설치되어 있어야 한다. 아니면 <experimental/filesystem>을 사용해야 한다.
#include <fstream>

class ItemManager
{
public:
    ItemManager(){};
    ~ItemManager(){};

    void Init();
    bool PreLoadAll();
    bool LoadJsonFile(const std::string& path, ItemInitData& itemData);
private:
    std::unordered_map<int, ItemInitData> m_item_initData;


};