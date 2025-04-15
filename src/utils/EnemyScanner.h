#pragma once

#include "RE/Skyrim.h"
#include <vector>

namespace EnemyScanner
{
    // Returns the reference IDs (FormIDs) of hostile NPCs near the player
    std::vector<RE::FormID> GetHostileNPCsNearPlayer(float radius = 1500.0f);
}
