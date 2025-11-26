#pragma once
#include "SKSE/SKSE.h"
#include "RE/Skyrim.h"
#include "NearbyAggroedActors.h"
#include "ConsoleUtils.h"
#include <vector>
#include <format>
#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

namespace EnemyScanner
{
    // Returns the reference IDs (FormIDs) of hostile NPCs near the player
    //std::vector<RE::FormID> 
    void    GetHostileNPCsNearPlayer(float radius, std::vector<RE::FormID>& enemies);
}
