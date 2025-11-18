#pragma once
#include "SKSE/SKSE.h"
#include "RE/Skyrim.h"
#include "NearbyAggroedActors.h"
#include "ConsoleUtils.h"
#include <vector>
#include <format>
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(std::format(__VA_ARGS__).c_str())

namespace EnemyScanner
{
    // Returns the reference IDs (FormIDs) of hostile NPCs near the player
    std::vector<RE::FormID> GetHostileNPCsNearPlayer(float radius = 1500.0f);
}
