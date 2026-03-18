#pragma once
#include "SKSE/SKSE.h"
#include "RE/Skyrim.h"
#include "NearbyAggroedActors.h"
#include "ConsoleUtils.h"
#include "ActorUtils.h"
#include "../Puppeteer.h"
#include <vector>
#include <format>
#include <cstring>
#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

namespace EnemyScanner
{
    // Returns the reference IDs (FormIDs) of hostile NPCs near the player
    void    GetHostileNPCsNearPlayer(float radius, std::vector<RE::FormID>& enemies);
}
