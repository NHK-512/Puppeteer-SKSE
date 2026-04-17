#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(std::format(__VA_ARGS__).c_str())
using namespace nlohmann;

//represents one record
struct combatRecord
{
    bool playerWins = false;
    int duration = 0;

    int dead = 0;
    int alive = 0;
    float groupDmg = 0;
    int ranger = 0;
    int vanguard = 0;
    int caster = 0;

    float melee = 0;
    float range = 0;
    float magic = 0;
    float physical = 0;
    float playerDmg = 0;
};

class CombatDataManager
{
private:

    json root;
    combatRecord pendingRecord;
    double rangerBias = 0.0f;
    double vanguardBias = 0.0f;
    double casterBias = 0.0f;

    const std::string filename = "Data/SKSE/Plugins/PuppeteerCombatRecord.json";
    void Save();
    
public:

    //CombatDataManager(const std::string& file);
    
    //Used to calculate Bias at anytime. Should be used right after 
    void calculateRoleBias();
    double getRangerBias() { return rangerBias; }
    double getVanguardBias() { return vanguardBias; }
    double getCasterBias() { return casterBias; }


    void CreateBlank();

    void Load();

    void CommitRecord();

    template<typename T>
    void SetValue(const std::string& field, T value)
    {
        if (field == "playerWins") pendingRecord.playerWins = value;
        else if (field == "duration") pendingRecord.duration = value;

        else if (field == "dead") pendingRecord.dead = value;
        else if (field == "alive") pendingRecord.alive = value;
        else if (field == "groupDmg") pendingRecord.groupDmg = value;

        else if (field == "melee") pendingRecord.melee = value;
        else if (field == "range") pendingRecord.range = value;
        else if (field == "magic") pendingRecord.magic = value;
        else if (field == "physical") pendingRecord.physical = value;
        else if (field == "playerDmg") pendingRecord.playerDmg = value;
        else if (field == "ranger") pendingRecord.ranger = value;
        else if (field == "vanguard") pendingRecord.vanguard = value;
        else if (field == "caster") pendingRecord.caster = value;
    }

    double GetAverage(const std::string& field);
};