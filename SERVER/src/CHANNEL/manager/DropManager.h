#pragma once
#include "common.h"
#include "DropInfos.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <random>

namespace fs = std::filesystem;

class DropManager
{
public:
    DropManager();
    ~DropManager(){};

    static DropManager *GetInstance();

    bool Init();
    bool PreLoadAll();
    bool LoadCommonDropFile(const fs::path& path);
    bool LoadUniqueDropFile(const fs::path& path);

    int CalculateWeight(std::vector<DropEntry>& entries);

    bool SelectDropItem(std::unordered_map<std::string, DropGroup>::iterator Groups, std::vector<DropResult>& dropItems);

    std::vector<DropResult> SetDropItem(std::string& commonGroup, std::string& uniqueGroup);


private:
    std::unordered_map<std::string, DropGroup> m_commonGroups;
    std::unordered_map<std::string, DropGroup> m_uniqueGroups;
    
   std::mt19937 m_gen;
private:
    static DropManager *m_instance;


};