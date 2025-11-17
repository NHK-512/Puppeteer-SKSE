#pragma once

#include <vector>

#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(std::format(__VA_ARGS__).c_str())


namespace RE {
    class Actor;
}

// Gets all nearby NPCs that are hostile to the player
std::vector<RE::Actor*> GetNearbyAggroedActors(float maxDistance/* = 5000.0f*/);
