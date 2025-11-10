#pragma once
#include "SKSE/SKSE.h"
#include "RE/Skyrim.h"
#include "NearbyAggroedActors.h"
#include <vector>
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(fmt::format(__VA_ARGS__).c_str())

namespace EnemyScanner
{
    // Returns the reference IDs (FormIDs) of hostile NPCs near the player
    std::vector<RE::FormID> GetHostileNPCsNearPlayer(float radius = 1500.0f);
}
