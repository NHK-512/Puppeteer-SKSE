#pragma once

#include <vector>

namespace RE {
    class Actor;
}

// Gets all nearby NPCs that are hostile to the player
std::vector<RE::Actor*> GetNearbyAggroedActors(float maxDistance = 5000.0f);
