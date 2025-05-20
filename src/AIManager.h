#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <unordered_set>

//#define CONSOLE_LOG RE::ConsoleLog::GetSingleton()->Print
 #define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(fmt::format(__VA_ARGS__).c_str())

static std::unordered_set<RE::FormID> previousEnemies;
static std::unordered_map<RE::FormID, char> currentRoles;
static RE::FormID currentLeaderID = 0;

std::vector<RE::FormID> enemies;

//input number is max distance as radius of the scanning area
//IMPORTANT: MAX DISTANCE AS TO BE HIGH (MINIMUM ~500)
float scanDistance = 5000;

//how many seconds that the cylce of the loop run for
int secondsPerCycle = 60;

namespace AIManager
{
    void Initialize();
    void RoleControl(RE::Actor* actor);
}
