#pragma once

#include <vector>

#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)


namespace RE {
    class Actor;
}

// Gets all nearby NPCs that are hostile to the player
std::vector<RE::Actor*> GetNearbyAggroedActors(float maxDistance/* = 5000.0f*/);
